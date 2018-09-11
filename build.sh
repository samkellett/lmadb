#!/bin/bash -e

PROJECT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

usage() {
  echo "Usage: $0 [-t <release|debug>] [-c] [-n]" 1>&2
  exit 0
}

# defaults
BUILD_TYPE="debug"
CLEAN=0
TESTS=1

while getopts ":t:c:n" O; do
  case "$O" in
    t)
      BUILD_TYPE="$OPTARG"
      ((BUILD_TYPE == "release" || BUILD_TYPE == "debug")) || usage
      ;;
    c)
      CLEAN=1
      ;;
    n)
      TESTS=0
      ;;
    *)
      usage
      ;;
  esac
done

echo "Build Type: ${BUILD_TYPE}"
echo "Clean Build: ${CLEAN}"

BUILD_DIR="$PROJECT_DIR/$BUILD_TYPE"
VENV_DIR="$BUILD_DIR/env"
if [[ $CLEAN == 1 ]]; then
	echo -n "Removing $BUILD_DIR... press 'y' to confirm: "
	read CONFIRM
	if [[ "$CONFIRM" == "y" ]]; then
		rm -r "$BUILD_DIR"
	else
		exit 1
	fi
fi

mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

VENV_LOADED=0
if [[ "$VIRTUAL_ENV" == "$VENV_DIR" ]]; then
  VENV_LOADED=1
fi

[[ -d "$VENV_DIR" ]] || virtualenv -p python3 --prompt "(lmadb-$BUILD_TYPE) " "$VENV_DIR"
source "$VENV_DIR/bin/activate"

[[ $VENV_LOADED == 1 ]] || echo "Run: source $VENV_DIR/bin/activate"

pip install --retries 1 --timeout 5 --upgrade pip
pip install --retries 1 --timeout 5 -r "$PROJECT_DIR/requirements.txt"
# TODO: this doesn't work when you install a new package.
pip freeze | grep -v lmadb > "$PROJECT_DIR/requirements.txt"

cmake "$PROJECT_DIR" \
	-GNinja \
	-DCMAKE_BUILD_TYPE=$BUILD_TYPE \
	-DCMAKE_EXPORT_COMPILE_COMMANDS=On \
	-DCMAKE_CXX_COMPILER=clang++ \
	-DCMAKE_C_COMPILER=clang

ninja
pip install $BUILD_DIR/libs/pylmadb

if [[ $TESTS == 1 ]]; then
  export CTEST_OUTPUT_ON_FAILURE=1
  export CTEST_PARALLEL_LEVEL=$(sysctl -n hw.ncpu)
  ninja test
fi
