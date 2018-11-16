import asyncio

async def tcp_echo_client(message, loop):
    reader, writer = await asyncio.open_connection('127.0.0.1', 8080,
                                                   loop=loop)

    print('Send: %r' % message)
    writer.write(message.encode())

    data = await reader.read(100)
    print('Received: %r' % data.decode())

    print('Close the socket')
    writer.close()


if __name__ == "__main__":
    message = 'Hello!'
    loop = asyncio.get_event_loop()

    task_q = []
    for i in range(10):
        t = loop.create_task(tcp_echo_client(message, loop))
        task_q.append(t)

    loop.run_forever()
    loop.close()
