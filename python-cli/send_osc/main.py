from pythonosc.udp_client import SimpleUDPClient
import argparse

def main():
    parser = argparse.ArgumentParser(description='Send an OSC Message.')
    parser.add_argument('address', type=str, help="The URI address")
    parser.add_argument('msg', nargs='*', help="Additional string/number arguments")
    parser.add_argument('--host', type=str, default='127.0.0.1', help="Host address (default: localhost)")
    parser.add_argument('--port', type=int, default=7777, help="Port number (default: 7777)")

    args = parser.parse_args()

    client = SimpleUDPClient(args.host, args.port)
    client.send_message(args.address, args.msg)

if __name__ == "__main__":
    main()
