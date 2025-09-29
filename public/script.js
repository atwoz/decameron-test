document.addEventListener('DOMContentLoaded', () => {
    // DOM Elements
    const tempSlider = document.getElementById('tempSlider');
    const tempValue = document.getElementById('tempValue');
    const intervalSlider = document.getElementById('intervalSlider');
    const intervalValue = document.getElementById('intervalValue');
    const statusBadge = document.getElementById('statusBadge');
    const powerState = document.getElementById('powerState');
    const currentTemp = document.getElementById('currentTemp');
    const targetTemp = document.getElementById('targetTemp');
    const mode = document.getElementById('mode');
    const fanSpeed = document.getElementById('fanSpeed');
    const lastUpdated = document.getElementById('lastUpdated');

    // State
    let sendInterval = 5; // seconds
    let targetTemperature = 24; // celsius
    let intervalTimer = null;
    let debounceTimer = null;

    // Send temperature to /execute endpoint
    async function sendTemperature(temperature) {
        try {
            const response = await fetch('/execute', {
                method: 'POST',
                headers: {
                    'Content-Type': 'application/json',
                },
                body: JSON.stringify({ temperature: temperature })
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
                lastUpdated.textContent = new Date().toLocaleTimeString();
            }
        } catch (error) {
            console.error('Error sending temperature:', error);
            statusBadge.textContent = 'Error';
            statusBadge.className = 'status-badge offline';
        }
    }

    // Temperature Slider Handler with debounce
    tempSlider.addEventListener('input', (e) => {
        targetTemperature = parseInt(e.target.value);
        tempValue.textContent = `${targetTemperature}°C`;
        targetTemp.textContent = `${targetTemperature}°C`;
    });

    // Send temperature when user releases the slider (change event)
    tempSlider.addEventListener('change', (e) => {
        targetTemperature = parseInt(e.target.value);
        console.log('Sending temperature:', targetTemperature);
        sendTemperature(targetTemperature);
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

    // Start Send Timer - periodically sends current temperature
    function startSendTimer() {
        intervalTimer = setInterval(() => {
            sendTemperature(targetTemperature);
            lastUpdated.textContent = new Date().toLocaleTimeString();
        }, sendInterval * 1000);
    }

    // Initialize
    tempValue.textContent = `${targetTemperature}°C`;
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
    sendTemperature(targetTemperature);

    // Start automatic sending
    startSendTimer();
});