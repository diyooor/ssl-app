// Include crypto-js for SHA-256 hashing
// <script src="https://cdn.jsdelivr.net/npm/crypto-js@4.1.1/crypto-js.js"></script>

const loginButton = document.getElementById('login');
const loginForm = document.getElementById('login-form');
const cancelButton = document.getElementById('cancel');
const submitButton = document.querySelector('#login-form button[type="submit"]');

loginButton.addEventListener('click', function(event) {
    event.preventDefault();
    loginForm.style.display = 'block';
});

cancelButton.addEventListener('click', function(event) {
    event.preventDefault();
    loginForm.style.display = 'none';
});

submitButton.addEventListener('click', async function(event) {
    event.preventDefault();

    // Get the form values
    const username = document.getElementById('username').value;
    const password = document.getElementById('password').value;

    // Hash the password using SHA-256
    const hashedPassword = CryptoJS.SHA256(password).toString(CryptoJS.enc.Hex);

    // Prepare the request body
    const requestBody = {
        username: username,
        password: hashedPassword
    };

    // Send the login request to the server
    const response = await fetch('/login', {
        method: 'POST',
        headers: {
            'Content-Type': 'application/json'
        },
        body: JSON.stringify(requestBody)
    });

    const result = await response.json();

    if (response.ok) {
        alert('Login successful!');
        // Perform post-login actions, like redirecting to the dashboard
    } else {
        alert('Login failed: ' + result.message);
    }
});

