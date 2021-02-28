const { exec } = require('child_process');
let stdOut;
data = {};
var normalexecount = 1;
var tappedexecount = 1;
const K = 2;

// executeNormal();
function executeNormal(){
    for (let i = 0; i < K; i++){
        exec("../isp 1 1 1 ../input.txt", (error, stdout, stderr) => {
            if (error) {
                console.log(`error: ${error.message}`);
                return;
            }
            if (stderr) {
                console.log(`stderr: ${stderr}`);
                return;
            }
            // console.log(`stdout: ${stdout}`);
            stdOut += stdout;
    
            var lines = stdOut.split('\n');
            var m;
            lines.map((line) => {
                var m_matches = line.match(/\.\.\/producer (\d+)/);
                var t_matches = line.match(/Execution time: (\d+)/);
                if (m_matches){
                    m = m_matches[1];
                }
    
                if (t_matches){
                    if (data[m]){
                        data[m] = (Number.parseFloat(t_matches[1]) + data[m])/2;
                    }
                    else
                        data[m] = Number.parseFloat(t_matches[1]);
                }
            });
            
            console.log(`Normal mode's ${normalexecount}'th time finished`);
            
            if (normalexecount >= K){
                console.log("Normal Mode, varying values of M")
                console.log(data);
                executeTapped();
            }
            normalexecount++;
        });
    }    
}
function executeTapped(){
    for (let i = 0; i < K; i++){
        exec("../isp 500 2 1 ../input.txt", (error, stdout, stderr) => {
            if (error) {
                console.log(`error: ${error.message}`);
                return;
            }
            if (stderr) {
                console.log(`stderr: ${stderr}`);
                return;
            }
            // console.log(`stdout: ${stdout}`);
            stdOut += stdout;
            
            var lines = stdOut.split('\n');
            var m;
            lines.map((line) => {
                var m_matches = line.match(/\.\.\/producer (\d+)/);
                var t_matches = line.match(/Execution time: (\d+)/);
                if (m_matches){
                    m = m_matches[1];
                }
                
                if (t_matches){
                    if (data[m]){
                        data[m] = (Number.parseFloat(t_matches[1]) + data[m])/2;
                    }
                    else
                    data[m] = Number.parseFloat(t_matches[1]);
                }
            });
            
            console.log(`Tapped mode's ${tappedexecount}'th time finished`);
            
            if (tappedexecount >= K){
                console.log("Tapped Mode, varying values of M, N = 50")
                console.log(data);
            }
            
            tappedexecount++;
        });
    }
}

const N = 7000000000;
executeTappedWithVaryingN(1000000000);
function executeTappedWithVaryingN(j){
    exec(`../isp ${j} 2 1 ../input2.txt`, (error, stdout, stderr) => {
        let curJ = j;

        if (error) {
            console.log(`error: ${error.message}`);
            return;
        }
        if (stderr) {
            console.log(`stderr: ${stderr}`);
            return;
        }
        // console.log(`stdout: ${stdout}`);
        stdOut += stdout;
        
        var lines = stdOut.split('\n');
        var m;
        lines.map((line) => {
            var m_matches = line.match(/\.\.\/producer (\d+)/);
            var t_matches = line.match(/Execution time: (\d+)/);
            if (m_matches){
                m = m_matches[1];
            }
            
            if (t_matches){
                if (data[m]){
                    data[m] = (Number.parseFloat(t_matches[1]) + data[m])/2;
                }
                else
                data[m] = Number.parseFloat(t_matches[1]);
            }
        });
        
        console.log(`Tapped mode's ${tappedexecount}'th time finished`);

        console.log(`Tapped Mode, varying values of N (${curJ}), M = 1000000`)
        console.log(data);

        tappedexecount++;

        if (j > 50){
            executeTappedWithVaryingN(j/1000);
        }
    });
}
