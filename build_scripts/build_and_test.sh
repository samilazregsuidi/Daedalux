echo "Building and testing daedalux"
# Stop and remove existing containers with the same name if they exist
docker stop daedalux
docker rm daedalux
# Build the image
docker build -t daedalux:latest --file Dockerfile --platform linux/arm64 --label daedalux .
echo "Image built successfully, now running the container..."
docker run --name daedalux daedalux:latest
