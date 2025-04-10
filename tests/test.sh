#! /usr/bin/env bash

bats_require_minimum_version 1.5.0

setup_io() {
    input_file="$BATS_TEST_TMPDIR/input.txt"
    output_file="$BATS_TEST_TMPDIR/output.txt"
}

create_io_files() {
    setup_io
    touch "$input_file" "$output_file"
}

################################################################################
# Params
################################################################################

@test "PARAMS: 0 params" {
    run ../pipex
    [[ "$status" != 0 ]]
}

@test "PARAMS: 1 params" {
    run ../pipex ""
    [[ "$status" != 0 ]]
}

@test "PARAMS: 2 params" {
    run ../pipex ""
    [[ "$status" != 0 ]]
}

@test "PARAMS: 3 params" {
    run ../pipex "" "" ""
    [[ "$status" != 0 ]]
}

@test "PARAMS: 4 params OK" {
    setup_io
    echo "hola como estas" > "$input_file"

    ../pipex "$input_file" "cat" "cat" "$output_file"

    diff "$input_file" "$output_file"
}

@test "PARAMS: 5 params" {
    run ../pipex "" "" "" "" ""
    [[ "$status" != 0 ]]
}

################################################################################
# Empty commands
################################################################################

@test "EMPTY COMMANDS: empty first command" {
    setup_io
    echo hola > "$input_file"

    run -0 ../pipex "$input_file" "" "cat" "$output_file"

    [[ -e "$output_file" ]]
    grep -El "Permission denied" <<< "$output"
}

@test "EMPTY COMMANDS: empty second command" {
    setup_io
    echo hola > "$input_file"

    run -1 ../pipex "$input_file" "cat" "" "$output_file"

    [[ -e "$output_file" ]]
    grep -El "Permission denied" <<< "$output"
}

################################################################################
# Files
################################################################################

@test "FILE INPUT: wrong permissions input file" {
    setup_io
    touch "$input_file"
    echo -e "hola\ncomo\nestas" > "$input_file"
    chmod 300 "$input_file"

    run ../pipex "$input_file" "grep hola" "sort" "$output_file"

    echo "$output" | grep -ql '.*input.txt: Permission denied'
    [[ "$status" -eq 0 ]]
    [[ -e "$output_file" ]]
}

@test "FILE INPUT: wrong permission output file" {
    setup_io
    touch "$input_file" "$output_file"
    echo -e "hola\ncomo\nestas" > "$input_file"
    chmod 500 "$output_file"

    run ../pipex "$input_file" "grep hola" "sort" "$output_file"

    echo "$output" |  grep -ql '.*output.txt: Permission denied'
    [[ "$status" -eq 1 ]]
}

@test "FILE INPUT: input file doesn't exist" {
    setup_io

    run -0 ../pipex "$input_file" "" "cat" "$output_file"

    echo $output
    [[ "$output" =~ .*input.txt:\ No\ such\ file\ or\ directory ]]
}

@test "FILE INPUT: output file can't be created" {
    setup_io
    touch "$input_file"
    chmod -200 $BATS_TEST_TMPDIR

    run -1 ../pipex "$input_file" "cat" "cat" "$output_file"

    chmod +200 $BATS_TEST_TMPDIR
    grep -lE '.*output.txt: Permission denied' <<< "$output"
}

@test "FILE INPUT: output file is a dir" {
    setup_io
    touch "$input_file"
    mkdir "$output_file"

    run -1 ../pipex "$input_file" "cat" "cat" "$output_file"

    grep -lE '.*output.txt: Is a directory' <<< "$output"
}

################################################################################
# Limit FD
################################################################################

@test "FDS: limit fd to get dup2 error" {
    create_io_files
    ulimit -n 7
    run -1 ../pipex "$input_file" cat cat "$output_file"
}

################################################################################
# Environment Path Shenanigans
################################################################################

test_path() {
    create_io_files
    echo -e "$3" > "$input_file"
    local pipex=${6-../pipex}
    PATH="$1" run "$2" "$pipex" "$input_file" "$4" "$5" "$output_file"
}

test_path_colon() {
    cat << EOF > $BATS_TEST_TMPDIR/exe
#!/bin/bash
/bin/cat
EOF
    cat << EOF > $BATS_TEST_TMPDIR/rev
#!/bin/bash
/bin/rev
EOF
    cp ../pipex $BATS_TEST_TMPDIR
    chmod +100 $BATS_TEST_TMPDIR/rev
    chmod +100 $BATS_TEST_TMPDIR/exe
    cd $BATS_TEST_TMPDIR
    test_path $1 -0 "hola\ncomo\nestas\n" exe rev ./pipex
    diff <(cat << EOF
aloh
omoc
satse

EOF
    ) "$output_file"
}

@test "PATH empty" {
    create_io_files
    PATH="" run -1 ../pipex "$input_file" ls cat "$output_file"
    grep -E 'ls: No such file or directory' <<< "$output"
    grep -E 'cat: No such file or directory' <<< "$output"
}

@test "PATH empty finds cwd" {
    create_io_files
    echo -e "hola\ncomo\nestas\n" > "$input_file"
    cat << EOF > $BATS_TEST_TMPDIR/exe
#!/bin/bash
/bin/cat
EOF
    cat << EOF > $BATS_TEST_TMPDIR/rev
#!/bin/bash
/bin/rev
EOF
    cp ../pipex $BATS_TEST_TMPDIR
    chmod +100 $BATS_TEST_TMPDIR/rev
    chmod +100 $BATS_TEST_TMPDIR/exe
    cd $BATS_TEST_TMPDIR
    PATH="" ./pipex "$input_file" exe rev "$output_file"
    diff <(cat << EOF
aloh
omoc
satse

EOF
    ) "$output_file"
}

@test "PATH: only colons" {
    test_path_colon "::::"
}

@test "PATH: trailing colon" {
    test_path_colon "/something/:"
}

@test "PATH: leading colon" {
    test_path_colon ":/something/"
}

@test "PATH: enodir fail" {
    test_path "/something:/doesntexist" -1 "doesntMattertext" ls man
    for cmd in ls man; do
        grep -E "$cmd: No such file or directory" <<< "$output"
    done
}

@test "PATH: enodir no fail" {
    test_path "/this_fails:/usr/bin" -0 "my text\nis great" cat "sed 's/t/r/g'"
    echo $output
    diff "$output_file" <(cat << EOF
my rexr
is grear
EOF
    )
}

@test "PATH: with combinations of slashes" {
    test_path "///////usr///////bin//////" -0 "What is the deal\nfriendo\n done?\nfriendo" cat\ -e "sort -u"
    diff "$output_file" <(cat << EOF
 done?$
friendo$
What is the deal$
EOF
    )
}

@test "PATH: unset" {
    create_io_files
    cat << EOF > $BATS_TEST_TMPDIR/exe
#!/bin/bash
/bin/cat
EOF
    cat << EOF > $BATS_TEST_TMPDIR/rev
#!/bin/bash
/bin/rev
EOF
    cp ../pipex $BATS_TEST_TMPDIR
    chmod +100 $BATS_TEST_TMPDIR/rev
    chmod +100 $BATS_TEST_TMPDIR/exe
    cd $BATS_TEST_TMPDIR
    run -1 env -u PATH ./pipex "$input_file" exe rev "$output_file"
    for cmd in rev exe; do
        grep -E "$cmd: No such file or directory" <<< "$output"
    done
}

@test "PATH: with space and newline" {
    local path_dir="$BATS_TEST_TMPDIR/my cursed"$'\n'"bin"
    mkdir "$path_dir"
    cat << EOF > "$path_dir/mycat"
#!/bin/bash
/bin/cat
EOF
    chmod +100 "$path_dir/mycat"
    test_path "$path_dir" -0 "this\nshould work in theory\nyeah" \
        mycat /bin/cat
    diff "$output_file" <(cat << EOF
this
should work in theory
yeah
EOF
    )
}

################################################################################
# Commands and Args Parsing
################################################################################

test_command_parsing() {
    create_io_files
    printf "%b" "$1" > "$input_file"
    run -0 ../pipex "$input_file" "$2" "$3" "$output_file"
    diff "$output_file" <(printf "%b" "$4")
}

@test "COMMAND PARSING: simple commands" {
    test_command_parsing "zo hola chamo como estas\n vamos a ver" \
        cat sort " vamos a ver\nzo hola chamo como estas\n"
}

@test "COMMAND PARSING: commands with args" {
    test_command_parsing "old man look at my life, im a lot like you were\n" \
        "sed \"-E\" 's/old/young/'" \
        'sed -E s/im\ a\ lot\ like\ you\ were/you\'\''re\ a\ lot\ like\ I\ was/' \
        "young man look at my life, you're a lot like I was\n"

}
