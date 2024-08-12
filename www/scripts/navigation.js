const homeButton = document.getElementById('home');
const aboutButton = document.getElementById('about');
const nav = document.querySelector('nav');
const content = document.getElementById('content');

// Ensure content is hidden and nav is in its default state on page load
window.onload = function() {
    content.style.display = 'none';
    content.style.opacity = '0';
    nav.classList.remove('shrink');
};

homeButton.addEventListener('click', function(event) {
    event.preventDefault();
    toggleContent('home');
});

aboutButton.addEventListener('click', function(event) {
    event.preventDefault();
    toggleContent('about');
});

function toggleContent(button) {
    if (button === 'home') {
        if (content.style.opacity === '1') {
            content.style.opacity = '0';
            setTimeout(() => {
                content.style.display = 'none';
                nav.classList.remove('shrink');
            }, 300);  // Matches the transition duration
        } else {
            content.style.display = 'block';
            setTimeout(() => content.style.opacity = '1', 10);
            nav.classList.add('shrink');
        }
    } else if (button === 'about') {
        content.style.opacity = '0';
        setTimeout(() => {
            content.style.display = 'none';
            nav.classList.remove('shrink');
        }, 300);  // Matches the transition duration
    }
}

