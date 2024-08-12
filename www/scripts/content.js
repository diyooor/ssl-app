// content.js

// Load the content into the placeholder
fetch('components/content.html')
    .then(response => response.text())
    .then(data => {
        document.getElementById('content-placeholder').innerHTML = data;
        hideContent(); // Ensure content is hidden on page load
    });

// Function to hide content
function hideContent() {
    const content = document.getElementById('content');
    content.style.display = 'none';
    content.style.opacity = '0';
}

// Function to show content
function showContent() {
    const content = document.getElementById('content');
    content.style.display = 'block';
    setTimeout(() => content.style.opacity = '1', 10); // Delayed opacity change for smooth transition
}

// Function to toggle content based on the button clicked
function toggleContent(button) {
    const nav = document.querySelector('nav');
    if (button === 'home') {
        if (document.getElementById('content').style.opacity === '1') {
            hideContent();
            nav.classList.remove('shrink');
        } else {
            showContent();
            nav.classList.add('shrink');
        }
    } else {
        hideContent();
        nav.classList.remove('shrink');
    }
}

