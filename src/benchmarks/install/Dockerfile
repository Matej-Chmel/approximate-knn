FROM python:3.9

WORKDIR /home/app
COPY benchmarks/run_algorithm.py ./
COPY benchmarks/install/requirements.txt ./

RUN python -m pip install -r requirements.txt
ENTRYPOINT ["python", "-u", "run_algorithm.py"]
