const API_BASE_URL = "http://127.0.0.1:8001";

// Max expected values for gauge calculation
const MAX_TEMP = 50;
const MAX_HUM = 100;
const MAX_MOIST = 100;
const MAX_CO2 = 2000;
const MAX_LIGHT = 2000;

function updateGauge(gaugeClass, value, maxVal) {
    const dasharray = 200; // Expected from CSS
    let percentage = value / maxVal;
    if (percentage > 1) percentage = 1;
    if (percentage < 0) percentage = 0;
    
    const offset = dasharray - (percentage * dasharray);
    const fill = document.querySelector(`.${gaugeClass}`);
    if (fill) {
        fill.style.strokeDashoffset = offset;
    }
}

async function fetchSensorData() {
    try {
        const response = await fetch(`${API_BASE_URL}/api/sensors`);
        if (response.ok) {
            const data = await response.json();
            
            if (data && !data.error) {
                // Update text values
                document.getElementById('val-temp').innerText = data.temperature.toFixed(1);
                document.getElementById('val-hum').innerText = data.humidity.toFixed(1);
                document.getElementById('val-moist').innerText = data.moisture;
                document.getElementById('val-air').innerText = data.co2; // CO2 mapped to Air Quality Gauge
                document.getElementById('val-light').innerText = data.light;

                // Update Gauges visually
                updateGauge('temp-fill', data.temperature, MAX_TEMP);
                updateGauge('hum-fill', data.humidity, MAX_HUM);
                updateGauge('moist-fill', data.moisture, MAX_MOIST);
                updateGauge('air-fill', data.co2, MAX_CO2);
                updateGauge('light-fill', data.light, MAX_LIGHT);

                // Make the connection dot glow solidly
                const connectionDot = document.getElementById('connection-dot');
                if(connectionDot) connectionDot.classList.add('connected');
                const connectionStatus = document.getElementById('connection-status');
                if(connectionStatus) connectionStatus.innerText = "Live Data";
            } else {
                console.warn("No data in database yet:", data.error);
                
                // Clear the default values to show it's actually empty!
                document.getElementById('val-temp').innerText = "0.0";
                document.getElementById('val-hum').innerText = "0.0";
                document.getElementById('val-moist').innerText = "0";
                document.getElementById('val-air').innerText = "0"; 
                document.getElementById('val-light').innerText = "0";

                updateGauge('temp-fill', 0, MAX_TEMP);
                updateGauge('hum-fill', 0, MAX_HUM);
                updateGauge('moist-fill', 0, MAX_MOIST);
                updateGauge('air-fill', 0, MAX_CO2);
                updateGauge('light-fill', 0, MAX_LIGHT);

                const connectionStatus = document.getElementById('connection-status');
                if(connectionStatus) connectionStatus.innerText = "DB Empty. Turn on ESP32!";
            }
        }
    } catch (error) {
        console.error("Error fetching sensor data:", error);
        
        const connectionDot = document.getElementById('connection-dot');
        if(connectionDot) connectionDot.classList.remove('connected');
        const connectionStatus = document.getElementById('connection-status');
        if(connectionStatus) connectionStatus.innerText = "Reconnecting...";
    }
}

// Fetch data as soon as the window loads
window.onload = () => {
    fetchSensorData();
    // Fetch new data every 3 seconds
    setInterval(fetchSensorData, 3000);
};
