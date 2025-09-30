document.addEventListener('DOMContentLoaded', () => {
    // DOM Elements
    const tempSlider = document.getElementById('tempSlider');
    const tempValue = document.getElementById('tempValue');
    const humiditySlider = document.getElementById('humiditySlider');
    const humidityValue = document.getElementById('humidityValue');
    const intervalSlider = document.getElementById('intervalSlider');
    const intervalValue = document.getElementById('intervalValue');
    const presenceToggle = document.getElementById('presenceToggle');
    const requestIndicator = document.getElementById('requestIndicator');
    const statusBadge = document.getElementById('statusBadge');
    const powerState = document.getElementById('powerState');
    const currentTemp = document.getElementById('currentTemp');
    const targetTemp = document.getElementById('targetTemp');
    const mode = document.getElementById('mode');
    const fanSpeed = document.getElementById('fanSpeed');
    const currentHumidity = document.getElementById('currentHumidity');
    const lastUpdated = document.getElementById('lastUpdated');

    // State
    let sendInterval = 5; // seconds
    let targetTemperature = 24; // celsius
    let targetHumidity = 50; // percent
    let presence = true; // default presence on
    let intervalTimer = null;
    let debounceTimer = null;

    // Flash indicator light
    function flashIndicator() {
        requestIndicator.classList.add('active');
        setTimeout(() => {
            requestIndicator.classList.remove('active');
        }, 300);
    }

    // Send data to /execute endpoint
    async function sendData(temperature, humidity, presenceState) {
        flashIndicator();
        try {
            const response = await fetch('/execute', {
                method: 'POST',
                headers: {
                    'Content-Type': 'application/json',
                },
                body: JSON.stringify({
                    temperature: temperature,
                    humidity: humidity,
                    presence: presenceState ? 1 : 0
                })
            });

            const data = await response.json();
            console.log('Temperature sent:', temperature, 'Response:', data);

            // Update UI with AC state from response
            if (data.status === 'success') {
                statusBadge.textContent = 'Online';
                statusBadge.className = 'status-badge online';

                // Update AC state display
                powerState.textContent = data.power || 'ON';
                currentTemp.textContent = `${data.currentTemp}°C`;
                targetTemp.textContent = `${data.targetTemp}°C`;
                mode.textContent = data.mode || 'Cool';
                fanSpeed.textContent = data.fanSpeed || 'Auto';
                currentHumidity.textContent = `${data.humidity}%`;
                lastUpdated.textContent = new Date().toLocaleTimeString();
            }
        } catch (error) {
            console.error('Error sending data:', error);
            statusBadge.textContent = 'Error';
            statusBadge.className = 'status-badge offline';
        }
    }

    // Temperature Slider Handler
    tempSlider.addEventListener('input', (e) => {
        targetTemperature = parseFloat(e.target.value);
        tempValue.textContent = `${targetTemperature}°C`;
        targetTemp.textContent = `${targetTemperature}°C`;
    });

    // Send data when user releases the temperature slider
    tempSlider.addEventListener('change', (e) => {
        targetTemperature = parseFloat(e.target.value);
        console.log('Sending temperature:', targetTemperature);
        sendData(targetTemperature, targetHumidity, presence);
    });

    // Humidity Slider Handler
    humiditySlider.addEventListener('input', (e) => {
        targetHumidity = parseFloat(e.target.value);
        humidityValue.textContent = `${targetHumidity}%`;
    });

    // Send data when user releases the humidity slider
    humiditySlider.addEventListener('change', (e) => {
        targetHumidity = parseInt(e.target.value);
        console.log('Sending humidity:', targetHumidity);
        sendData(targetTemperature, targetHumidity, presence);
    });

    // Presence Toggle Handler
    presenceToggle.addEventListener('change', (e) => {
        presence = e.target.checked;
        console.log('Presence changed to:', presence);
        sendData(targetTemperature, targetHumidity, presence);
    });

    // Send Interval Slider Handler
    intervalSlider.addEventListener('input', (e) => {
        sendInterval = parseInt(e.target.value);
        intervalValue.textContent = `${sendInterval}s`;

        // Restart the send timer with new interval
        clearInterval(intervalTimer);
        startSendTimer();
        console.log('Set send interval to:', sendInterval, 'seconds');
    });

    // Start Send Timer - periodically sends current data
    function startSendTimer() {
        intervalTimer = setInterval(() => {
            sendData(targetTemperature, targetHumidity, presence);
            lastUpdated.textContent = new Date().toLocaleTimeString();
        }, sendInterval * 1000);
    }

    // Initialize
    tempValue.textContent = `${targetTemperature}°C`;
    humidityValue.textContent = `${targetHumidity}%`;
    intervalValue.textContent = `${sendInterval}s`;
    targetTemp.textContent = `${targetTemperature}°C`;

    // Set initial mock data
    powerState.textContent = 'ON';
    currentTemp.textContent = '24.0°C';
    mode.textContent = 'Cool';
    fanSpeed.textContent = 'Auto';
    statusBadge.textContent = 'Ready';
    statusBadge.className = 'status-badge';
    lastUpdated.textContent = new Date().toLocaleTimeString();

    // Initial send
    sendData(targetTemperature, targetHumidity, presence);

    // Start automatic sending
    startSendTimer();
});