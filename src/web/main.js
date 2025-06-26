import { loadWebAssemblyModule} from './modules/webassembly.js';
import { initializeFieldSelection } from './modules/fieldSelection.js';
import { parsePolynomials } from './modules/polynomialParser.js';
import { solve } from './modules/solver.js';
import { copyResults } from './modules/utils.js';

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