# PIQTree2 devcontainer Development Environment

## Prerequisites
- Docker installed on your system.
- Visual Studio Code (VS Code) with `Docker` and `Remote - Containers` extensions installed.
- A local clone of your GitHub fork of the PIQTree2 repository on your host machine.
- SSH Agent running on your host with your SSH keys (for you github account) added

## Setting Up the Development Environment

1. **Clone the Repository**:
   `git clone git@github.com:cogent3/PiQTree2.git`  Clone this repository to your local machine.
   `cd PiQTree2` Change directory to the repository folder.
   `git submodule init` to initialize the submodules
   `git submodule update` to pull in the submodules

2. **Open in VS Code**:
   Open the project folder in Visual Studio Code.
   `code .` Open the project folder in VS Code.

3. **Start Development Container**:
   - Use the "Remote-Containers: Open Folder in Container" command in VS Code.
   - This will build and start the development container based on the `.devcontainer` configuration.

## SSH Agent Forwarding

To access private repositories or other resources that require SSH authentication, ensure your SSH Agent is running:

1. **Start SSH Agent (if not already running)**:
   - On Windows: Run `start-ssh-agent.cmd`.
   - On macOS/Linux: Run `eval $(ssh-agent)` and then `ssh-add`.

2. **Verify SSH Keys**:
   - Ensure that your SSH keys are added to the agent with `ssh-add -l`.

3. **Using SSH in the Container**:
   - The SSH Agent forwarding in `.devcontainer` allows the container to use your SSH keys.

### Working with the Container

- The container includes all necessary tools and dependencies pre-installed.
- You can modify the Dockerfile or `devcontainer.json` to add additional tools.

### Troubleshooting

- **SSH Agent Issues**: Ensure the SSH Agent is running and keys are added on your host machine.
- **Container Build Errors**: Check the Dockerfile and `devcontainer.json` for configuration issues.

## Build IQTree2 in the container

```sh   
cd /workspaces/PiQTree2
rm -rf build/ && mkdir build && cd build
cmake ..
make
```

## Run IQTree2 with test data in the container

```sh
/workspaces/PiQTree2/build/iqtree2 -s /workspaces/PiQTree2/test_scripts/test_data/example.phy
```

Make sure to test each of these steps to confirm they work as expected. If there are any additional configurations or dependencies for Jupyter or IQTREE2, you should include those in the Dockerfile or document them as part of the setup process.