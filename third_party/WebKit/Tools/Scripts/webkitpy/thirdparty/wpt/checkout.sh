#!/bin/bash
#
# Removes ./wpt/ directory containing the reduced web-platform-tests tree and
# starts a new checkout. Only files in WPTWhiteList are retained. The revisions
# getting checked out are defined in WPTHeads.

DIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )
cd $DIR

TARGET_DIR=$DIR/wpt
REMOTE_REPO="https://chromium.googlesource.com/external/w3c/web-platform-tests.git"

function clone {
  # First line is the main repo HEAD.
  WPT_HEAD=$(head -n 1 $DIR/WPTHeads)

  # Remove existing repo if already exists.
  [ -d "$TARGET_DIR" ] && rm -rf $TARGET_DIR

  # Clone the main repository.
  git clone $REMOTE_REPO $TARGET_DIR
  cd $TARGET_DIR && git checkout $WPT_HEAD

  # Starting from the 2nd line of WPTWhiteList, we read and checkout submodules.
  tail -n+2 $DIR/WPTHeads | while read dir submodule commit; do
    cd $TARGET_DIR/$dir && \
      git submodule update --init $submodule && \
      cd $TARGET_DIR/$dir/$submodule && \
      git checkout $commit
  done
}

function reduce {
  # Remove all except white-listed.
  cd $TARGET_DIR
  find . -type f | grep -Fxvf ../WPTWhiteList | xargs -n 1 rm
  find . -empty -type d -delete
}

actions="clone reduce"
[ "$1" != "" ] && actions="$@"

for action in $actions; do
  type -t $action >/dev/null || (echo "Unknown action: $action" 1>&2 && exit 1)
  $action
done

# TODO(burnik): Handle the SSL certs and other configuration.
