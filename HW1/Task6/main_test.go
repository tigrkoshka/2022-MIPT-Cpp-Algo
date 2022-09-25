package main

import (
	"bytes"
	"fmt"
	"os"
	"os/exec"
	"regexp"
	"strings"
	"testing"

	"github.com/stretchr/testify/require"
)

var (
	casesSep       = regexp.MustCompile(`\n#+\n`)
	testCaseRegexp = regexp.MustCompile(
		`Input:

(?s:(?P<input>.*))

Output:

fifo:

(?s:(?P<fifo>.*))

lifo:

(?s:(?P<lifo>.*))`,
	)
)

type caseConfig struct {
	Input      string
	FIFOOutput string
	LIFOOutput string
}

func getGroupsMap(t *testing.T, re *regexp.Regexp, str string) map[string]string {
	t.Helper()

	groups := re.FindStringSubmatch(str)
	names := re.SubexpNames()
	require.Equal(t, len(names), len(groups))

	res := make(map[string]string, len(groups))
	for i := 1; i < len(names); i++ {
		res[names[i]] = groups[i]
	}

	return res
}

func getTestCases(t *testing.T, data string) []*caseConfig {
	t.Helper()

	casesStr := casesSep.Split(data, -1)

	cases := make([]*caseConfig, 0, len(casesStr))
	for _, caseStr := range casesStr {
		groups := getGroupsMap(t, testCaseRegexp, caseStr)
		cases = append(
			cases, &caseConfig{
				Input:      strings.TrimSpace(groups["input"]),
				FIFOOutput: strings.TrimSpace(groups["fifo"]),
				LIFOOutput: strings.TrimSpace(groups["lifo"]),
			},
		)
	}

	return cases
}

func run(t *testing.T, input, expectOutput string, flags []string) {
	t.Helper()

	out := new(bytes.Buffer)

	cmd := exec.Command("./broker", flags...)
	cmd.Stdin = bytes.NewReader([]byte(input))
	cmd.Stdout = out

	err := cmd.Run()
	require.NoError(t, err)
	require.Equal(t, expectOutput, strings.TrimSpace(out.String()))
}

func TestBroker(t *testing.T) {
	t.Parallel()

	data, err := os.ReadFile("tests.txt")
	require.NoError(t, err)

	cases := getTestCases(t, string(data))
	for i, c := range cases {
		i, c := i, c // capture range variables
		t.Run(
			fmt.Sprintf("CASE-%d", i), func(t *testing.T) {
				t.Parallel()
				t.Run(
					"FIFO", func(t *testing.T) {
						t.Parallel()
						run(t, c.Input, c.FIFOOutput, []string{"-t", "fifo"})
					},
				)
				t.Run(
					"LIFO", func(t *testing.T) {
						t.Parallel()
						run(t, c.Input, c.LIFOOutput, []string{"-t", "lifo"})
					},
				)
			},
		)
	}
}
