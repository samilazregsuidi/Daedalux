echo "Building and testing daedalux"
# Stop and remove existing containers with the same name if they exist
docker stop daedalux
docker rm daedalux
# Build the image
docker build -t daedalux:latest --file Dockerfile --platform linux/amd64 --label daedalux .
echo "Image built successfully, now running tests in the container"
docker run --name daedalux daedalux:latest
