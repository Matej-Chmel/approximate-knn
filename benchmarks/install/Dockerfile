FROM ubuntu:18.04

RUN apt-get update
RUN apt-get install -y build-essential python3.9

WORKDIR /home/app
COPY requirements.txt run_algorithm.py ./
RUN python -m pip install -r requirements.txt

ENTRYPOINT ["python", "-u", "run_algorithm.py"]
