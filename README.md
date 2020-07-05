This is an extremely primitive stream video player based on ffmpeg libraries and QT GUI. 
It's possible to build and run it locally or inside docker container. 

For local build the process is straightforward:

    mkdir build
    cd build
    cmake ..
    make

As a result, 'ffqt' file should appear inside 'build' directory


Alternative approach is to use dockerized build. 

First, the docker container should be created using included Dockerfile. To simplify the process, the helper script, named 'docker_build_container.sh' is included in the root of the source tree.

To initiate the build process, another script, named 'docker_build_ffqt.sh', is included. It maps the local directory to the container as a source, and puts all build artifacts into the 'output' directory which will be created in the source root.

Docker container uses the latest ubuntu build. The host system may have different version, or even different distro. In this case the additional script which allows to run the binary is provided, it names 'docker_run_ffqt.sh'

So the build (and run) process using docker can look like this:
    
    sh docker_build_container.sh
    sh docker_build_ffqt.sh
    sh docker_run_ffqt.sh


