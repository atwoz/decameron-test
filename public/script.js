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
    let refetchInterval = 5; // seconds
    let targetTemperature = 24; // celsius
    let intervalTimer = null;

    // Temperature Slider Handler
    tempSlider.addEventListener('input', (e) => {
        targetTemperature = parseInt(e.target.value);
        tempValue.textContent = `${targetTemperature}°C`;
        // TODO: Send temperature change to AC API
        console.log('Set temperature to:', targetTemperature);
    });

    // Refetch Interval Slider Handler
    intervalSlider.addEventListener('input', (e) => {
        refetchInterval = parseInt(e.target.value);
        intervalValue.textContent = `${refetchInterval}s`;

        // Restart the refetch timer with new interval
        clearInterval(intervalTimer);
        startRefetchTimer();
        console.log('Set refetch interval to:', refetchInterval, 'seconds');
    });

    // Fetch AC State Function
    async function fetchACState() {
        try {
            // TODO: Replace with actual API endpoint
            // For now, simulating API call with mock data
            const mockData = {
                power: Math.random() > 0.5 ? 'ON' : 'OFF',
                currentTemp: (20 + Math.random() * 10).toFixed(1),
                targetTemp: targetTemperature,
                mode: ['Cool', 'Heat', 'Fan', 'Auto'][Math.floor(Math.random() * 4)],
                fanSpeed: ['Low', 'Medium', 'High', 'Auto'][Math.floor(Math.random() * 4)],
                online: Math.random() > 0.2
            };

            // Simulate API delay
            await new Promise(resolve => setTimeout(resolve, 300));

            updateUI(mockData);
        } catch (error) {
            console.error('Error fetching AC state:', error);
            statusBadge.textContent = 'Error';
            statusBadge.className = 'status-badge offline';
        }
    }

    // Update UI with AC State
    function updateUI(data) {
        // Update status badge
        if (data.online) {
            statusBadge.textContent = 'Online';
            statusBadge.className = 'status-badge online';
        } else {
            statusBadge.textContent = 'Offline';
            statusBadge.className = 'status-badge offline';
        }

        // Update state info
        powerState.textContent = data.power;
        currentTemp.textContent = `${data.currentTemp}°C`;
        targetTemp.textContent = `${data.targetTemp}°C`;
        mode.textContent = data.mode;
        fanSpeed.textContent = data.fanSpeed;
        lastUpdated.textContent = new Date().toLocaleTimeString();
    }

    // Start Refetch Timer
    function startRefetchTimer() {
        intervalTimer = setInterval(() => {
            fetchACState();
        }, refetchInterval * 1000);
    }

    // Initialize
    tempValue.textContent = `${targetTemperature}°C`;
    intervalValue.textContent = `${refetchInterval}s`;

    // Initial fetch
    fetchACState();

    // Start automatic refetching
    startRefetchTimer();
});