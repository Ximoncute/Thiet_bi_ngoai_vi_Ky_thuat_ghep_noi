const mqtt = require('mqtt')
const mongoose = require('mongoose')
const { XMLParser } = require('fast-xml-parser')

const parser = new XMLParser()

// ===== CONNECT MONGODB =====
mongoose.connect('mongodb://127.0.0.1:27017/iot_db')
  .then(() => console.log('MongoDB Connected'))
  .catch(err => console.log('Mongo Error:', err))

// ===== SCHEMA =====
const sensorSchema = new mongoose.Schema({
  device_id: String,
  temperature: Number,
  humidity: Number,
  timestamp: Number,
  led_temp_state: Boolean,
  led_hum_state: Boolean,
  mode: String
}, { timestamps: true })

const Sensor = mongoose.model('Sensor', sensorSchema)

// ===== MQTT =====
const client = mqtt.connect('mqtt://broker.emqx.io')

client.on('connect', () => {
  console.log('MQTT Connected')
  client.subscribe('iot/+/data')
})

// ===== NORMALIZATION FUNCTION =====
function normalizeData(rawMessage) {
  let data = null
  const messageStr = rawMessage.toString().trim()

  // Try JSON first
  if (messageStr.startsWith('{') || messageStr.startsWith('[')) {
    try {
      data = JSON.parse(messageStr)
      if (Array.isArray(data)) data = data[0]
    } catch (e) {
      console.log('JSON Parse Error')
    }
  }
  // Try XML if not JSON
  else if (messageStr.startsWith('<')) {
    try {
      const xmlObj = parser.parse(messageStr)
      // Standardize XML structure (e.g. <data><temp>...</temp></data> -> {temp: ...})
      data = xmlObj.data || xmlObj.sensor || xmlObj
    } catch (e) {
      console.log('XML Parse Error')
    }
  }

  if (!data) return null

  // Normalize field names (Example: t -> temperature, h -> humidity)
  const normalized = {
    device_id: data.device_id || data.id || 'unknown_device',
    temperature: data.temperature ?? data.temp ?? data.t,
    humidity: data.humidity ?? data.hum ?? data.h,
    timestamp: data.timestamp ?? Date.now(),
    led_temp_state: data.led_temp_state ?? data.led_t ?? false,
    led_hum_state: data.led_hum_state ?? data.led_h ?? false,
    mode: data.mode ?? 'unknown'
  }

  return normalized
}

// ===== HANDLE DATA =====
client.on('message', async (topic, message) => {
  try {
    const data = normalizeData(message)

    if (!data || data.device_id === 'unknown_device') {
      console.log('Bo qua data khong hop le')
      return
    }

    // Check at least one valid sensor reading
    if (data.temperature === undefined && data.humidity === undefined) {
      console.log('Thieu du lieu sensor')
      return
    }

    console.log('DATA NORMALIZED:', data)

    // ===== SAVE DB =====
    const newData = new Sensor(data)
    await newData.save()
    console.log('Saved to MongoDB')

  } catch (err) {
    console.log('Error:', err.message)
  }
})