
Here’s a README for your project that includes the necessary steps for cloning the repository, building and running the Docker container, as well as instructions for installing requirements, running the WebSocket feature, and the rest of your setup:

Deribit Order Management System
This project allows you to interact with the Deribit API for placing orders, canceling orders, modifying orders, and viewing current positions via WebSocket. It includes functionalities for querying open orders, managing account positions, and receiving real-time market data.

Requirements
Before running the project, you need to install the following dependencies:

Docker: To build and run the container.
CMake: A tool to manage the build process in a compiler-independent way.
Make: To build the application inside the Docker container.
You can install Docker by following the instructions on the official Docker website.

Setup Instructions
1. Clone the Repository
Clone the repository using the following command:

bash
Copier le code
git clone git@github.com:salaheddinebellafrouh/deribitfinal.git
cd deribitfinal
2. Build the Docker Image
Once you have cloned the repository, build the Docker image by running the following command:

bash
Copier le code
docker build -t deribit-order-management .
This will build the image with the tag deribit-order-management.

3. Run the Docker Container
Run the Docker container with the following command:

bash
Copier le code
docker run -it deribit-order-management
This command starts the container interactively and runs the application inside the container.

4. Run Inside the Docker Container
Once inside the container, you can build and run the application by executing the following commands:

bash
Copier le code
make run

This will compile and run the application inside the Docker container.

Usage
Once the application is running, you can interact with the menu to perform various actions such as:

Place an Order: Place an order on the Deribit exchange.
Modify an Order: Edit an existing order.
Get Open Orders: View your currently open orders.
View Current Positions: Check your current positions in the market.
Cancel an Order: Cancel an existing order.
Real-time Market Data Streaming: Stream real-time market data using WebSockets.
Exit: Exit the application.
WebSocket Setup
The application supports real-time market data streaming using WebSockets. To enable this feature, select the Real-time market data streaming option from the menu. This will connect you to the WebSocket server and start receiving live market data updates.

Authentication
When running the program, you will be prompted to enter your Client ID and Secret ID to authenticate with the Deribit API. These credentials are necessary to interact with the Deribit private endpoints.

License
This project is licensed under the MIT License - see the LICENSE file for details.

Adjustments
Feel free to replace "your-repository-url" and "your-application-name" with the appropriate information for your repository and application.
