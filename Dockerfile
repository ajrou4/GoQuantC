# Use Ubuntu as the base image
FROM ubuntu:22.04 as builder

# Set environment variables to avoid interactive prompts during installation
ENV DEBIAN_FRONTEND=noninteractive

# Update package list and install dependencies
RUN apt-get update && apt-get install -y \
    cmake \
    git \
    curl \
    libcurl4-openssl-dev \
    libboost-all-dev \
    build-essential \
    && apt-get clean

# Clone Json for C++ modern 
RUN git clone git@github.com:nlohmann/json.git json-develop
# Clone WebSocket++ repository from GitHub
RUN git clone https://github.com/zaphoyd/websocketpp.git /websocketpp

# Create a working directory
WORKDIR /usr/src/app

# Copy your project files into the container
COPY . .

# Build your application
RUN mkdir -p build && cd build && \
    cmake -DWEBSOCKETPP_DIR=/websocketpp .. && \
    make

# Use a runtime image based on Ubuntu for the final image
FROM ubuntu:22.04

# Install runtime dependencies
RUN apt-get update && apt-get install -y \
    libboost-system1.74.0 \
    libcurl4 \
    && apt-get clean

# Copy the built application from the builder stage
COPY --from=builder /usr/src/app/build/your_application /usr/local/bin/

# Expose the WebSocket server port
EXPOSE 9002

# Run the application
CMD ["your_application"]
