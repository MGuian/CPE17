import paho.mqtt.client as mqtt

# MQTT broker settings
BROKER = "192.168.1.27"   # Change to your broker's IP
PORT = 1883
TOPIC = "home/livingroom/temperature"

# Callback when connected to broker
def on_connect(client, userdata, flags, rc):
    if rc == 0:
        print("✅ Connected to MQTT Broker!")
        client.subscribe(TOPIC)
    else:
        print(f"❌ Connection failed with code {rc}")

# Callback when message is received
def on_message(client, userdata, msg):
    print(f"📩 Topic: {msg.topic} | Message: {msg.payload.decode()}")

# Create MQTT client instance
client = mqtt.Client()

client.on_connect = on_connect
client.on_message = on_message

# Connect to broker
print("🔌 Connecting to broker...")
client.connect(BROKER, PORT, keepalive=60)

# Blocking loop to process network traffic and callbacks
client.loop_forever()
