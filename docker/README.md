# PiQTree2 Docker Development Environment

## Prerequisites
- Docker installed on your system.
- Visual Studio Code (VS Code) with Docker and Remote - Containers extensions installed.
- A local clone of your GitHub fork of the IQTREE2 repository on your host machine.
- SSH keys set up for GitHub to allow cloning and pushing to the repository.

## Build the Docker Image

From the root of you repository clone run the following command to build the docker image

```sh
docker build --tag piqtree2 -f .\docker\DockerFile .
```

## Run the Docker Container

To run the docker container, run the following command from the root of your repository clone:

```sh
docker run -it -d -v ${PWD}:/iqtree2 -v $env:USERPROFILE/.ssh/github:/root/.ssh/id_rsa -p 3000:3000 --name piqtree2 piqtree2  /bin/bash
```

This will open up a bash terminal inside the container. The -v option mounts the current directory on the host machine to the /iqtree2 woprking directory in the container. The -p option maps ports in the container to ports in the host machine.  Port 3000 is used for Visual Studio Code's remote debugging features.

## Build IQTree2 in the container

```sh   
cd /iqtree2
rm -rf build/ && mkdir build && cd build
cmake ..
make
```

## Run IQTree2 with test data in the container

```sh
/iqtree2/build/iqtree2 -s /iqtree2/test_scripts/test_data/example.phy
```


## Develop in the Container with VS Code

After the container is running, you can use VS Code to attach to the running container, which allows you to develop directly inside the container.

- Open the Command Palette (Ctrl+Shift+P).
- Select Remote-Containers: Attach to Running Container....
- Choose the piqtree2 container from the list.

## Committing and Pushing Changes

While working within the container, you can commit changes and push to your GitHub repository either using VS Code's source control features or by using the command line.

### Using VS Code

- Open the Source Control panel in VS Code.
- Stage your changes by clicking on the '+' sign next to the changed files.
- Enter a commit message and press Ctrl+Enter to commit the changes.
- Push the changes by clicking on the '...' button and selecting 'Push'.

### Using Command Line

Run the following commands within the container's terminal:

```sh
cd /iqtree2
git add .
git commit -m "Your commit message"
git push origin main
```

Replace main with the appropriate branch name if you are working on a different branch.

## Jupyter Notebook Integration

To use Jupyter notebooks for automating IQTREE2 with PiQTree2, a Jupyter server must be started on port 8888. This port will also be mapped to the host OS.

### Running Jupyter Server

To start a Jupyter server inside the container from the VS code terminal, enter the following command 

```bash
jupyter notebook --ip 0.0.0.0 --port 8888 --no-browser --allow-root &
```

To stop a running Jupyter server, run the following command in the container's terminal:

```bash
jupyter notebook stop 8888
```

With this setup, you can access the Jupyter server from your host OS by navigating to http://localhost:8888 in your web browser. The Jupyter server will provide a token for the first-time setup, which you can find in the output of the terminal where you started the server. You can also find the token by running the following command in the container's terminal:

```bash
jupyter notebook list
```

### Notes

- The entrypoint.sh script is executed when the container starts and is responsible for setting up the SSH agent and adding your SSH private key.
- The Docker container runs an interactive bash shell by default.


Make sure to test each of these steps to confirm they work as expected. If there are any additional configurations or dependencies for Jupyter or IQTREE2, you should include those in the Dockerfile or document them as part of the setup process.