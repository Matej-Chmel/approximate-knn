FROM python:3.9

WORKDIR /home/app
COPY benchmarks/requirements.txt benchmarks/run_algorithm.py ./
RUN python -m pip install -r requirements.txt

ENTRYPOINT ["python", "-u", "run_algorithm.py"]
