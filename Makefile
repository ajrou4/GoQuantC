# Define the image name and tag
IMAGE_NAME = order-management
TAG = latest

# Define the build directory
BUILD_DIR = build

# Default target, build the Docker image
all: build

# Build the Docker image
build:
	docker build -t $(IMAGE_NAME):$(TAG) .

# Run the Docker container (Expose port 9002)
run:
	docker run -p 9002:9002 $(IMAGE_NAME):$(TAG)

# Clean up the Docker image
clean:
	docker rmi $(IMAGE_NAME):$(TAG)

# Remove all unused Docker images, containers, volumes, and networks
prune:
	docker system prune -af

# Display the Docker image status
status:
	docker images $(IMAGE_NAME):$(TAG)

# Run the container in detached mode
detached:
	docker run -d -p 9002:9002 $(IMAGE_NAME):$(TAG)

# Execute shell in the running container
exec:
	docker exec -it $(shell docker ps -q -f "ancestor=$(IMAGE_NAME):$(TAG)") /bin/bash
