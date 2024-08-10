function loadContent(page) {
    fetch(page)
        .then(response => response.text())
        .then(data => {
            document.getElementById('main-content').innerHTML = data;
        })
        .catch(error => {
            document.getElementById('main-content').innerHTML = '<p>Error loading content.</p>';
            console.error('Error loading content:', error);
        });
}

// Load the default page (home) when the page first loads
document.addEventListener('DOMContentLoaded', () => {
    loadContent('home.html');
});

