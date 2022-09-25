#include <cstdint>
#include <deque>
#include <fstream>
#include <iostream>
#include <sstream>
#include <unordered_map>
#include <utility>
#include <vector>

using namespace std::literals::string_literals;

const std::string usage = "Broker is a tool for aggregating stock operations\n"
                          "\n"
                          "Usage: \n"
                          "\n"
                          "    broker [-h | --help] [(-i | --input) <file>] [(-o | --output) <file>] [(-t | --type) <broker_type>]\n"
                          "\n"
                          "Options:\n"
                          "    -h, --help\n"
                          "       Output this page\n"
                          "\n"
                          "    -i, --input\n"
                          "       Specify the input file\n"
                          "       Default: stdin\n"
                          "\n"
                          "    -o, --out\n"
                          "       Specify the output file\n"
                          "       Default: stdout\n"
                          "\n"
                          "    -t, --type\n"
                          "       Specify the broker type\n"
                          "       Options: fifo, lifo\n"
                          "       Default: fifo";

struct OpInfo {
    uint32_t op_id;
    double price;
    uint32_t n;

    OpInfo(uint32_t op_id_, double price_, uint32_t n_) : op_id(op_id_), price(price_), n(n_) {}
};

enum DealType {
    LongDeal,
    ShortDeal
};

class DealInfo {
private:
    [[nodiscard]] double GetProfit() const {
        double profit = n * (close_price - open_price);
        if (type == ShortDeal) {
            profit = -profit;
        }

        return profit;
    }

    [[nodiscard]] std::string ToString() const {
        std::ostringstream ss;
        ss << share_id << " " << n << " " << open_price << " ";

        if (unclosed) {
            ss << "- - ";
        } else {
            ss << close_price << " " << GetProfit() << " ";
        }

        switch (type) {
            case LongDeal:
                ss << "long ";
                break;
            case ShortDeal:
                ss << "short ";
                break;
        }

        ss << open_op_id;
        if (!unclosed) {
            ss << " " << close_op_id;
        }

        return ss.str();
    }

    friend std::ostream &operator<<(std::ostream &out, const DealInfo &deal);
    friend bool operator<(const DealInfo &lhs, const DealInfo &rhs);

public:
    DealInfo(std::string share_id_, uint32_t n_, DealType type_,
             double open_price_, uint32_t open_op_id_) : share_id(std::move(share_id_)), n(n_), type(type_),
                                                         open_price(open_price_), close_price(0),
                                                         open_op_id(open_op_id_), close_op_id(0),
                                                         unclosed(true) {}

    DealInfo(std::string share_id_, uint32_t n_, DealType type_,
             double open_price_, double close_price_,
             uint32_t open_op_id_, uint32_t close_op_id_) : share_id(std::move(share_id_)), n(n_), type(type_),
                                                            open_price(open_price_), close_price(close_price_),
                                                            open_op_id(open_op_id_), close_op_id(close_op_id_),
                                                            unclosed(false) {}

private:
    std::string share_id;
    uint32_t n;
    DealType type;
    double open_price;
    double close_price;
    uint32_t open_op_id;
    uint32_t close_op_id;
    bool unclosed;
};

std::ostream &operator<<(std::ostream &out, const DealInfo &deal) {
    out << deal.ToString();
    return out;
}

bool operator<(const DealInfo &lhs, const DealInfo &rhs) {
    if (lhs.share_id != rhs.share_id) {
        return lhs.share_id < rhs.share_id;
    }

    if (lhs.unclosed != rhs.unclosed) {
        return !lhs.unclosed;
    }

    return lhs.close_op_id < rhs.close_op_id;
}

enum BrokerType {
    FIFO,
    LIFO,
};

class Broker {
private:
    enum Mode {
        BuyMode,
        SellMode,
    };

    static DealType GetDealType(Mode mode) {
        switch (mode) {
            case BuyMode:
                return ShortDeal;
            case SellMode:
                return LongDeal;
        }
    }

    [[nodiscard]] std::deque<OpInfo> &GetAligned(const std::string &share_id, Mode mode) {
        switch (mode) {
            case BuyMode:
                return ops[share_id].first;
            case SellMode:
                return ops[share_id].second;
        }
    }

    [[nodiscard]] std::deque<OpInfo> &GetOpposed(const std::string &share_id, Mode mode) {
        switch (mode) {
            case BuyMode:
                return ops[share_id].second;
            case SellMode:
                return ops[share_id].first;
        }
    }

    // Precondition: ops.contains(share_id)
    [[nodiscard]] bool ShouldErase(const std::string &share_id) const {
        auto needed_ops = ops.at(share_id);
        return needed_ops.first.empty() && needed_ops.second.empty();
    }

    void RegisterOp(const std::string &share_id, const OpInfo &op, Mode mode) {
        std::deque<OpInfo> &aligned = GetAligned(share_id, mode);
        std::deque<OpInfo> &opposed = GetOpposed(share_id, mode);

        if (opposed.empty()) {
            aligned.emplace_back(op);
            return;
        }

        uint32_t left_n = op.n;
        while (left_n > 0 && !opposed.empty()) {
            OpInfo &curr_opposed = type == FIFO ? opposed.front() : opposed.back();
            uint32_t curr_n = std::min(curr_opposed.n, left_n);

            deals.emplace_back(share_id, curr_n, GetDealType(mode),
                               curr_opposed.price, op.price,
                               curr_opposed.op_id, op.op_id);

            left_n -= curr_n;
            curr_opposed.n -= curr_n;
            if (curr_opposed.n == 0) {
                switch (type) {
                    case FIFO:
                        opposed.pop_front();
                        break;
                    case LIFO:
                        opposed.pop_back();
                        break;
                }
            }
        }

        if (left_n > 0) {
            aligned.emplace_back(op.op_id, op.price, left_n);
        }

        if (ShouldErase(share_id)) {
            ops.erase(share_id);
        }
    }

public:
    explicit Broker(const BrokerType type_) : type(type_) {}

    void Buy(const std::string &share_id, const OpInfo &op) {
        return RegisterOp(share_id, op, BuyMode);
    }

    void Sell(const std::string &share_id, const OpInfo &op) {
        RegisterOp(share_id, op, SellMode);
    }

    // Once
    void Close() {
        for (const auto &[share_id, left_ops]: ops) {
            for (const auto &op: left_ops.first) {
                deals.emplace_back(share_id, op.n, LongDeal, op.price, op.op_id);
            }

            for (const auto &op: left_ops.second) {
                deals.emplace_back(share_id, op.n, ShortDeal, op.price, op.op_id);
            }
        }

        std::sort(deals.begin(), deals.end());
    }

    void PrintDeals(std::ostream &out) const {
        for (const DealInfo &deal: deals) {
            out << deal << "\n";
        }
    }

private:
    const BrokerType type;

    // share_id -> [buy_ops, sell_ops]
    //
    // invariant:
    //   - for each specific key exactly one of {buy_ops, sell_ops} is empty at all times
    std::unordered_map<std::string, std::pair<std::deque<OpInfo>, std::deque<OpInfo>>> ops;
    std::vector<DealInfo> deals;
};

int main(int argc, char **argv) {
    size_t nargs = argc;

    std::ifstream fin;
    std::ofstream fout;
    BrokerType type = FIFO;
    for (size_t i = 1; i < nargs; ++i) {
        if ("-h"s == argv[i] || "--help"s == argv[i]) {
            std::cout << usage << "\n";
            return 0;
        }

        if ("-i"s == argv[i] || "--input"s == argv[i]) {
            if (nargs < i + 2) {
                std::cout << "Error: input unspecified\n"
                             "\n"
                             "See usage: broker --help\n";
            }

            fin = std::ifstream(argv[++i]);
            continue;
        }

        if ("-o"s == argv[i] || "--output"s == argv[i]) {
            if (nargs < i + 2) {
                std::cout << "Error: output unspecified\n"
                             "\n"
                             "See usage: broker --help\n";
            }

            fout = std::ofstream(argv[++i]);
            continue;
        }

        if ("-t"s == argv[i] || "--type"s == argv[i]) {
            if (nargs < i + 2) {
                std::cout << "Error: type unspecified\n"
                             "\n"
                             "See usage: broker --help\n";
            }

            ++i;
            if ("fifo"s == argv[i]) {
                type = FIFO;
            } else if ("lifo"s == argv[i]) {
                type = LIFO;
            } else {
                std::cout << "Error: unknown broker type: " << argv[i] << "\n"
                                                                          "\n"
                          << "See usage: broker --help\n";
            }
            continue;
        }

        std::cout << "Unknown option " << argv[i] << "\n"
                                                     "\n"
                  << "See usage: broker --help\n";
    }

    std::istream &in = fin.is_open() ? fin : std::cin;
    std::ostream &out = fout.is_open() ? fout : std::cout;

    Broker broker(type);

    uint32_t id;
    while (in >> id) {
        std::string mode, share_id;
        uint32_t n;
        double price;
        in >> mode >> share_id >> n >> price;

        OpInfo op(id, price, n);
        if (mode == "buy") {
            broker.Buy(share_id, op);
        } else {
            broker.Sell(share_id, op);
        }
    }

    broker.Close();
    broker.PrintDeals(out);
    return 0;
}
