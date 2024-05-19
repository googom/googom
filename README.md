# Googom

## What is Googom?

Googom is a robust single-stop data platform utilizing Kappa architecture for real-time data processing and transformation. It is designed to manage vast amounts of data efficiently with a strong focus on modularity, security, and high availability.

## Key Components

1. **HTTP Server** - Manages JSON formatted data interactions via HTTP protocols.
2. **TCP Server** - Handles binary data transfers over TCP, utilizing Boost libraries for optimal performance.
3. **Data Writer** - Writes and validates data against schemas, storing it in memory for fast access.
4. **Data Dumper** - Executes periodic data dumps to disk, based on configurable intervals, to ensure data persistence.
5. **Governance** - Controls permissions for data access, serving as the backbone for security and compliance.

## Features

- **Data Governance**: Implements policies for data quality, user permissions, and schema versioning.
- **Topic Management**: Supports various topic operations including appending, updating, and producing events for schema changes.
- **High Performance**: Designed for low-latency and high-throughput environments, minimizing data movement.
- **Security**: Advanced security mechanisms for data encryption, and secure communication channels.

## Getting Started

1. **Installation**: Clone the repository and build using CMake.
2. **Configuration**: Configure the application using the provided sample config files.
3. **Running Googom**: Start the application following the detailed startup guide in the docs.

## Contributing

We welcome contributions! For guidelines on how to contribute, please refer to CONTRIBUTING.md.

## License

Licensed under Apache License 2.0. See [LICENSE.md](LICENSE.md) for more details.

## Acknowledgments

- Thanks to the Boost community and all contributors who make Googom a powerful tool for data management.
