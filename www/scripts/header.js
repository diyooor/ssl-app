// header.js

// Load the header into the placeholder
fetch('components/header.html')
    .then(response => response.text())
    .then(data => {
        document.getElementById('header-placeholder').innerHTML = data;
        setupNavigation(); // Setup navigation after header is loaded
        checkSession(); // Ensure session check is done after loading header and setting up navigation
    });

function setupNavigation() {
    const homeButton = document.getElementById('home');
    homeButton.addEventListener('click', function(event) {
        event.preventDefault();
        toggleContent('home');
    });
}

function checkSession() {
    const remembered = localStorage.getItem('rememberMe');
    const username = localStorage.getItem('username');

    if (remembered && username) {
        // If "Remember Me" is checked, restore session
        updateWelcomeMessage(username); // Update welcome message with the username
    } else {
        // Otherwise, check session from the server
        fetch('/', {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json'
            },
            body: JSON.stringify({ command: 'validate_session' })
        })
        .then(response => response.json())
        .then(data => {
            if (data.username) {
                updateWelcomeMessage(data.username); // Update welcome message with the username
            }
        })
        .catch(error => console.error('Error validating session:', error));
    }
}

function updateWelcomeMessage(username) {
    const welcomeMessage = document.querySelector('#content h1');
    welcomeMessage.textContent = username ? `Welcome ${username}` : 'Welcome';
}

