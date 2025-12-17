# Rakurai DEX Connector Demo Application

This is a demo application for the **rakurai-dex-connector** library. It demonstrates how to integrate and use the DEX connector in your applications.

## Prerequisites

- Docker installed on your system
- Debian package for `rakurai-dex-connector` (will be provided via Slack/Email)

## Setup Instructions

### 1. Build Docker Container

Build the Docker image using the provided Dockerfile:

```bash
docker build -t rakurai-dex-connector-demo .
```

### 2. Run Docker Container with Volume Mapping

Run the container with volume mapping to mount your project directory:

```bash
docker run --name dex-connector-container -it -v $(pwd):/dex-connector-demo-app rakurai-dex-connector-demo
```

This command:
- Runs the container in interactive mode (`-it`)
- Maps the current directory to `/dex-connector-demo-app` in the container (`-v $(pwd):/dex-connector-demo-app`)

### 3. Install the Debian Package

Once inside the container:

1. Place the provided Debian package (`.deb` file) in the root folder of the project
2. Install the package using:

```bash
dpkg -i <debian-package-name>.deb
```

Replace `<debian-package-name>` with the actual name of the Debian package file.

### 4. Build the Application

Navigate to the project directory and build the application:

```bash
mkdir build
cd build
cmake ..
make
```

### 5. Configure and Run

1. Copy the dummy configuration file to the build directory:

```bash
cp ../utilities/config.json .
```

2. Run the executable:

```bash
./rakurai-dex-connector-demo
```

You should start receiving updates from the DEX connector.

## Configuration

The demo application uses a configuration file (`config.json`) located in the `utilities/` directory. A dummy configuration file is provided as a template. Copy this file to your build directory before running the application.

