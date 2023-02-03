#!/bin/sh
activate () {
    source ".venv/bin/activate"
}
if [ ! -d ".venv" ]; then
    python -m venv .venv
    activate
    pip install --upgrade pip
    pip install -r requirements.txt
fi


activate
#ninja
python 'pre_test_suite.py'  --binary '../build/rash'
deactivate
