const benben = require('./benben_cpp_api');
benben.init_ux((error) => {
  console.log(error || "exited cleanly");
});
module.exports = benben;