const number_team_red_i = document.getElementById("number_team_red_i");
const number_team_red_d = document.getElementById("number_team_red_d");
const red_btn = document.getElementById("red_btn");
const red_input = document.getElementById("red_input");

const number_team_green_i = document.getElementById("number_team_green_i");
const number_team_blue_d = document.getElementById("number_team_green_d");
const number_team_green_input = document.getElementById(
  "number_team_green_input"
);
const green_btn = document.getElementById("green_btn");
const green_input = document.getElementById("green_input");

const start_input = document.getElementById("start_input");
const start_btn = document.getElementById("start_btn");
const stopS = document.getElementById("stop");
const reset = document.getElementById("reset");

const bomb_input = document.getElementById("bomb_input_time");
const bomb_btn = document.getElementById("bomb_btn_time");

// When the user clicks on the button, open the modal
red_btn.onclick = async (e) => {
  await fetch("/member/count/1?l=" + red_input.value);
};
number_team_red_i.onclick = async () => {
  await fetch("/member/count/increment/1");
};
number_team_red_d.onclick = async () => {
  await fetch("/member/count/decrement/1");
};

green_btn.onclick = async (e) => {
  await fetch("/member/count/2?l=" + green_input.value);
};
number_team_green_i.onclick = async () => {
  await fetch("/member/count/increment/2");
};
number_team_green_d.onclick = async () => {
  await fetch("/member/count/decrement/2");
};

start_btn.onclick = async () => {
  await fetch("/start?l=" + start_input.value);
  start_input.value = undefined;
};
stopS.onclick = async () => {
  await fetch("/stop");
};
reset.onclick = async () => {
  await fetch("/reset");
};

bomb_btn.onclick = async () => {
  await fetch("/bomb/time?l=" + bomb_input.value);
  bomb_input.value = undefined;
};
