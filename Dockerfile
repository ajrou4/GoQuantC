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

# Clone JSON for C++ modern (using HTTPS instead of SSH)
RUN git clone https://github.com/nlohmann/json.git /usr/local/json-develop

# Clone WebSocket++ repository from GitHub
RUN git clone https://github.com/zaphoyd/websocketpp.git /usr/local/websocketpp

# Create a working directory
WORKDIR /usr/src/app

# Copy your project files into the container
COPY . .

# Build your application
RUN mkdir -p build && cd build && \
    cmake -DWEBSOCKETPP_DIR=/usr/local/websocketpp -DJSON_DIR=/usr/local/json-develop .. && \
    make

# Use a runtime image based on Ubuntu for the final image
FROM ubuntu:22.04

# Install runtime dependencies
RUN apt-get update && apt-get install -y \
    libboost-system1.74.0 \
    libcurl4 \
    && apt-get clean

# Create a non-root user
RUN useradd -m appuser

# Copy the built application from the builder stage
COPY --from=builder /usr/src/app/build/main /usr/local/bin/

# Set the working directory
WORKDIR /home/appuser

# Switch to non-root user
USER appuser

# Expose the WebSocket server port
EXPOSE 9002

# Run the application
CMD ["a.out"]