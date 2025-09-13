import { getParsedPolynomials } from './main.js';
import { displayPolynomials } from './polynomialParser.js';

export function initializeFieldSelection() {
    const primeInput = document.getElementById('prime-input');
    
    if (primeInput) {
        primeInput.style.display = 'none';
    }
    
    document.querySelectorAll('input[name="field"]').forEach(radio => {
        radio.addEventListener('change', function() {
            if (this.value === 'Fp') {
                primeInput.style.display = 'block';
            } else {
                primeInput.style.display = 'none';
            }
            
            // Update polynomial display immediately when field changes
            const parsedPolynomials = getParsedPolynomials();
            if (parsedPolynomials.length > 0) {
                displayPolynomials(parsedPolynomials);
            }
        });
    });

    // Enhanced prime input functionality
    const primeInputField = document.getElementById('prime');
    if (primeInputField) {
        primeInputField.value = '2';
        
        // Remove any non-digit characters on input
        primeInputField.addEventListener('input', function() {
            let value = this.value.replace(/[^0-9]/g, '');
            this.value = value;
            
            if (value) {
                validatePrime(parseInt(value));
                const fieldRadio = document.querySelector('input[name="field"]:checked');
                const parsedPolynomials = getParsedPolynomials();
                if (fieldRadio && fieldRadio.value === 'Fp' && parsedPolynomials.length > 0) {
                    displayPolynomials(parsedPolynomials);
                }
            } else {
                clearPrimeValidation();
            }
        });
        
        // Validate on blur - only set default if completely empty
        primeInputField.addEventListener('blur', function() {
            if (this.value === '') {
                this.value = '2';
                validatePrime(2);

                // Update polynomial display when prime is set to default
                const fieldRadio = document.querySelector('input[name="field"]:checked');
                const parsedPolynomials = getParsedPolynomials();
                if (fieldRadio && fieldRadio.value === 'Fp' && parsedPolynomials.length > 0) {
                    displayPolynomials(parsedPolynomials);
                }
            }
        });
        
        // Enhanced keyboard controls
        primeInputField.addEventListener('keydown', function(e) {
            // Allow: backspace, delete, tab, escape, enter
            if ([8, 9, 27, 13, 46].indexOf(e.keyCode) !== -1 ||
                // Allow: Ctrl+A, Ctrl+C, Ctrl+V, Ctrl+X, Ctrl+Z
                (e.ctrlKey && [65, 67, 86, 88, 90].indexOf(e.keyCode) !== -1) ||
                // Allow: home, end, left, right, up, down arrows
                (e.keyCode >= 35 && e.keyCode <= 40)) {
                return;
            }
            
            // Ensure that it is a number and stop the keypress
            if ((e.shiftKey || (e.keyCode < 48 || e.keyCode > 57)) && (e.keyCode < 96 || e.keyCode > 105)) {
                e.preventDefault();
            }
        });
        
        // Handle paste events
        primeInputField.addEventListener('paste', function(e) {
            e.preventDefault();
            const paste = (e.clipboardData || window.clipboardData).getData('text');
            const numericValue = paste.replace(/[^0-9]/g, '');
            if (numericValue) {
                this.value = numericValue;
                validatePrime(parseInt(numericValue));
                const fieldRadio = document.querySelector('input[name="field"]:checked');
                const parsedPolynomials = getParsedPolynomials();
                if (fieldRadio && fieldRadio.value === 'Fp' && parsedPolynomials.length > 0) {
                    displayPolynomials(parsedPolynomials);
                }
            }
        });
    }
}

function validatePrime(num) {
    const primeInputField = document.getElementById('prime');
    const primeValidation = document.getElementById('prime-validation') || createPrimeValidation();
    
    if (!primeInputField) return;
    
    if (num < 2) {
        primeValidation.textContent = '⚠️ Must be ≥ 2';
        primeValidation.className = 'prime-validation warning';
        primeInputField.classList.add('warning');
        primeInputField.classList.remove('valid');
        return false;
    } else if (isPrime(num)) {
        primeValidation.textContent = '✅ Valid prime';
        primeValidation.className = 'prime-validation valid';
        primeInputField.classList.remove('warning');
        primeInputField.classList.add('valid');
        return true;
    } else {
        primeValidation.textContent = '⚠️ Not a prime number';
        primeValidation.className = 'prime-validation warning';
        primeInputField.classList.add('warning');
        primeInputField.classList.remove('valid');
        return false;
    }
}

function clearPrimeValidation() {
    const primeInputField = document.getElementById('prime');
    const primeValidation = document.getElementById('prime-validation');
    
    if (primeValidation) {
        primeValidation.textContent = '';
        primeValidation.className = 'prime-validation';
    }
    
    if (primeInputField) {
        primeInputField.classList.remove('warning', 'valid');
    }
}

function createPrimeValidation() {
    const primeInput = document.getElementById('prime-input');
    const validation = document.createElement('div');
    validation.id = 'prime-validation';
    validation.className = 'prime-validation';
    primeInput.appendChild(validation);
    return validation;
}

export function isPrime(n) {
    if (n < 2) return false;
    if (n === 2) return true;
    if (n % 2 === 0) return false;
    
    for (let i = 3; i * i <= n; i += 2) {
        if (n % i === 0) return false;
    }
    return true;
}