// footer.js

// Load the footer and login form into the placeholders
fetch('components/footer.html')
    .then(response => response.text())
    .then(data => {
        document.getElementById('footer-placeholder').innerHTML = data;

        // Load the login form after the footer is loaded
        return fetch('components/login_form.html');
    })
    .then(response => response.text())
    .then(data => {
        document.body.insertAdjacentHTML('beforeend', data); // Append the login form to the body

        // Setup login/logout after loading the login form
        setupLoginLogout();
        checkSession();
    })
    .catch(error => console.error('Error loading footer or login form HTML:', error));

function setupLoginLogout() {
    const loginButton = document.getElementById('login');
    const logoutButton = document.getElementById('logout');
    const loginForm = document.getElementById('login-form');
    const cancelButton = document.getElementById('cancel');
    const submitButton = document.querySelector('#login-form button[type="submit"]');
    const rememberMeCheckbox = document.getElementById('remember-me');

    if (loginButton) {
        loginButton.addEventListener('click', function(event) {
            event.preventDefault();
            loginForm.style.display = 'block';
        });
    }

    if (cancelButton) {
        cancelButton.addEventListener('click', function(event) {
            event.preventDefault();
            loginForm.style.display = 'none';
        });
    }

    if (submitButton) {
        submitButton.addEventListener('click', async function(event) {
            event.preventDefault();

            // Get the form values
            const username = document.getElementById('username').value;
            const password = document.getElementById('password').value;
            const rememberMe = rememberMeCheckbox.checked;

            // Hash the password using SHA-256
            const hashedPassword = CryptoJS.SHA256(password).toString(CryptoJS.enc.Hex);

            // Prepare the request body
            const requestBody = {
                command: 'login',
                username: username,
                password: hashedPassword,
                remember_me: rememberMe
            };

            // Send the login request to the server
            const response = await fetch('/', {
                method: 'POST',
                headers: {
                    'Content-Type': 'application/json'
                },
                body: JSON.stringify(requestBody)
            });

            if (response.ok) {
                // Hide the login form and replace the login button with the logout button
                loginForm.style.display = 'none';
                loginButton.style.display = 'none';
                logoutButton.style.display = 'block';
                updateWelcomeMessage(username); // Update welcome message with the username

                if (rememberMe) {
                    localStorage.setItem('rememberMe', 'true');
                    localStorage.setItem('username', username);
                }
            } else {
                alert('Login failed');
            }
        });
    }

    if (logoutButton) {
        logoutButton.addEventListener('click', async function(event) {
            event.preventDefault();

            // Send the logout request to the server
            const response = await fetch('/', {
                method: 'POST',
                headers: {
                    'Content-Type': 'application/json'
                },
                body: JSON.stringify({ command: 'logout' })
            });

            if (response.ok) {
                // Hide the logout button and show login button again
                logoutButton.style.display = 'none';
                loginButton.style.display = 'block';

                updateWelcomeMessage(''); // Clear the username from the content title
                localStorage.removeItem('rememberMe');
                localStorage.removeItem('username');
            } else {
                alert('Logout failed');
            }
        });
    }
}

async function checkSession() {
    const loginButton = document.getElementById('login');
    const logoutButton = document.getElementById('logout');

    const remembered = localStorage.getItem('rememberMe');
    const username = localStorage.getItem('username');

    if (remembered && username) {
        // If "Remember Me" is checked, restore session
        loginButton.style.display = 'none';
        logoutButton.style.display = 'block';
        updateWelcomeMessage(username); // Update welcome message with the username
    } else {
        // Otherwise, check session from the server
        const response = await fetch('/', {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json'
            },
            body: JSON.stringify({ command: 'validate_session' })
        });

        if (response.ok) {
            const data = await response.json();
            loginButton.style.display = 'none';
            logoutButton.style.display = 'block';
            updateWelcomeMessage(data.username); // Update welcome message with the username
        } else {
            logoutButton.style.display = 'none';
            loginButton.style.display = 'block';
        }
    }
}

function updateWelcomeMessage(username) {
    const welcomeMessage = document.querySelector('#content h1');
    welcomeMessage.textContent = username ? `Welcome ${username}` : 'Welcome';
}

