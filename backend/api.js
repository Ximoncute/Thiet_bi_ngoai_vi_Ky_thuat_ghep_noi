const express = require('express')
const mongoose = require('mongoose')
const app = express()

// ===== FIX CORS =====
const cors = require('cors')
app.use(cors())

// ===== CONNECT DB =====
mongoose.connect('mongodb://127.0.0.1:27017/iot_db')
  .then(() => console.log('MongoDB Connected'))

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

// ===== API =====
app.get('/api/latest', async (req, res) => {
  const data = await Sensor.findOne().sort({ createdAt: -1 })
  res.json(data)
})

// ===== SERVE FRONTEND =====
const path = require('path');
// Phục vụ file HTML tĩnh
app.get('/', (req, res) => {
  res.sendFile(path.join(__dirname, '../arduino_ide/code_phan_mem/HTML/index.html'));
});

// ===== START =====
app.listen(3000, () => console.log('API running at http://localhost:3000'))