import { loadWebAssemblyModule} from './webassembly.js';
import { initializeFieldSelection } from './fieldSelection.js';
import { parsePolynomials } from './polynomialParser.js';
import { solve } from './solver.js';
import { copyResults } from './utils.js';

export let Module = null;
export let parsedPolynomials = [];

export function setModule(module) {
    Module = module;
}

export function setParsedPolynomials(polynomials) {
    parsedPolynomials = polynomials;
}

export function getModule() {
    return Module;
}

export function getParsedPolynomials() {
    return parsedPolynomials;
}

window.parsePolynomials = parsePolynomials;
window.solve = solve;
window.copyResults = copyResults;

document.addEventListener('DOMContentLoaded', function() {
    initializeFieldSelection();
    loadWebAssemblyModule();
});

const worker = new Worker('./modules/worker.js', { type: 'module' });