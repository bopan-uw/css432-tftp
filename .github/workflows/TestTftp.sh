#!/bin/bash

echo "starting server..."
./tftp-server &

echo "starting client..."
./tftp-client < input.txt

diff input.txt output.txt
diff_result=$?
if [ $diff_result -eq 1 ]
then
  echo "expected output:"
  echo "$(<input.txt)"
  echo "actual output:"
  echo "$(<output.txt)"
  exit 1
else
  echo "test passed"
fi
