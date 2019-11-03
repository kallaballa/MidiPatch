function noteOn(e) {
  e.target.classList.add('playing');
}

function removeTransition(e) {
  if (e.propertyName !== 'transform') return;
  e.target.classList.remove('playing');
}

const keys = Array.from(document.querySelectorAll('.key'));
keys.forEach(key => key.addEventListener('transitionend', removeTransition));
keys.forEach(key => key.addEventListener('click', noteOn));
keys.forEach(key =>
  keyboardJS.bind($(key).data("combo"), function(e) {
    alert("hi");
    noteOn(e);
    e.preventDefault();
  });
);

