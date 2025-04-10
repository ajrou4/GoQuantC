IMAGE_NAME = order-management
TAG = latest

.PHONY: build run clean

build:
	docker build -t $(IMAGE_NAME):$(TAG) .

run:
	docker run -p 9002:9002 $(IMAGE_NAME):$(TAG)

clean:
	docker rmi $(IMAGE_NAME):$(TAG)

detached:
	docker run -d -p 9002:9002 $(IMAGE_NAME):$(TAG)

exec:
	docker exec -it $$(docker ps -q -f "ancestor=$(IMAGE_NAME):$(TAG)") /bin/bash





