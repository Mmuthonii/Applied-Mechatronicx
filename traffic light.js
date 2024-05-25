const lights = document.querySelectorAll('.light');
let currentLightIndex = 0;

function switchLight() {
    lights[currentLightIndex].style.opacity = 0.5;
    currentLightIndex = (currentLightIndex + 1) % lights.length;
    lights[currentLightIndex].style.opacity = 1.0;
}

const button = document.querySelector('.button');

button.addEventListener('click', () => {
    switchLight();
});
