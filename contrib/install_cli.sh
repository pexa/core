 #!/usr/bin/env bash

 # Execute this file to install the pexa cli tools into your path on OS X

 CURRENT_LOC="$( cd "$(dirname "$0")" ; pwd -P )"
 LOCATION=${CURRENT_LOC%Pexa-Qt.app*}

 # Ensure that the directory to symlink to exists
 sudo mkdir -p /usr/local/bin

 # Create symlinks to the cli tools
 sudo ln -s ${LOCATION}/Pexa-Qt.app/Contents/MacOS/pexad /usr/local/bin/pexad
 sudo ln -s ${LOCATION}/Pexa-Qt.app/Contents/MacOS/pexa-cli /usr/local/bin/pexa-cli
