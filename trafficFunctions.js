document.querySelectorAll('.traffic_light div').forEach(light => {
    light.addEventListener('click', function() {
        if (this.classList.contains('red')) {
            turnOnRed();
        } else if (this.classList.contains('yellow')) {
            turnOnYellow();
        } else if (this.classList.contains('green')) {
            turnOnGreen();
        }
    });
});

let yellowInterval;
let lights = ['red', 'yellow', 'green'];
let currentLightIndex = 0;

function turnOnRed() {
    clearInterval(yellowInterval);
    switchLights('green', 'yellow', 'red');
}

function turnOnYellow() {
    clearInterval(yellowInterval);
    switchLights('red', 'yellow', 'green');
    yellowInterval = setInterval(() => {
        document.querySelector('.yellow').classList.toggle('active');
    }, 500);
}

function turnOnGreen() {
    clearInterval(yellowInterval);
    switchLights('yellow', 'red', 'green');
}

function switchLights(prevLight, currentLight, nextLight) {
    document.querySelectorAll('.traffic_light div').forEach(light => light.classList.remove('active'));
    document.querySelector('.' + currentLight).classList.add('active');
    currentLightIndex = lights.indexOf(currentLight);
}

