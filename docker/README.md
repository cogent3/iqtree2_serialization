## Build docker image in VS Code powershell terminal

```sh
docker build --tag piqtree2 -f .\docker\DockerFile .
```

## Run docker container in VS Code powershell terminal 

```sh
docker run -it -d -v ${PWD}:/iqtree2 -v $env:USERPROFILE/.ssh/github:/root/.ssh/id_rsa -p 3000:3000 --name piqtree2 piqtree2  /bin/bash
```

## build IQTree2 in VS Code bash terminal inside container

```sh
cd /iqtree2
rm -rf build/
mkdir
cd build
cmake ..
make
```

## Run IQTree2 with test data

```sh
cd /iqtree2/test_scripts/test_data
/iqtree2/build/iqtree2 -s example.phy
```