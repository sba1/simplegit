
# compare time betwen git and git2

COMMAND="ls-files"
TIMES=1000
EXEC_GIT="$(pwd)/tests/git_repo/git"
EXEC_GIT2="$(pwd)/bin/git2"
TEST_DIR="$(pwd)/tests/git_repo/"



function header() {
echo "* $1 : "
printf "\t"
}

function footer() {
echo ""
}


header "Machine"
uname -a
footer

header "GIT version"
cd tests/git_repo/
cat .git/$(git symbolic-ref HEAD)
cd ../../
footer

header "GIT2 version"
cat .git/$(git symbolic-ref HEAD)
footer


here=$(pwd)
cd $TEST_DIR
function testit() {
	i=0
	while test $i -lt $TIMES
	do
		$EXEC_GIT $COMMAND 2>&1 1>/dev/null
		i=$(($i + 1))
	done
}

header "GIT time"
echo "execute $TIMES times '$EXEC_GIT $COMMAND' in $TEST_DIR"
time testit $EXEC_GIT
footer


header "GIT2 time"
echo "execute $TIMES times '$EXEC_GIT2 $COMMAND' in $TEST_DIR"
time testit $EXEC_GIT2
footer


cd $here




