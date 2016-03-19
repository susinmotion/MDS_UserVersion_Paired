#!/bin/bash
make
python run.py

python read_match.py | ./secondtrie

