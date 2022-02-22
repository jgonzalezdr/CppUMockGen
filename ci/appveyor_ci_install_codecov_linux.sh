#!/bin/bash

mkdir $CODECOV_HOME
curl -s https://codecov.io/bash > $CODECOV_HOME/codecov
chmod +x $CODECOV_HOME/codecov
