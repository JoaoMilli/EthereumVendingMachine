// SPDX-License-Identifier: GPL-3.0

pragma solidity >=0.7.0 <0.9.0;

/**
 * @title Storage
 * @dev Store & retrieve value in a variable
 * @custom:dev-run-script ./scripts/deploy_with_ethers.ts
 */
contract Storage {

    uint256 number;
    address owner;

    /**
     * @dev Store value in variable
     * @param num value to store
     */
    function store(uint256 num) public {
        number = num;
    }

    /**
     * @dev Return value 
     * @return value of 'number'

     0xd9145CCE52D386f254917e481eB44e9943F39138 < contrato
     0x5B38Da6a701c568545dCfcB03FcB875f56beddC4 < deployer
     */
    function retrieve() public view returns (uint256){
        return number;
    }

    modifier onlyOwner() {
        require (owner == msg.sender);
        _;
    }

    modifier endTransaction() {
        _;
        //
    }

    constructor() public {
        owner = msg.sender;
    }

    function only_Owner() {
        if (msg.sender == owner) return true;
        return false;
    }

    function withdraw() public onlyOwner {
        // require (owner == msg.sender);
        // transfere balance para to
        // transfer(balanceFrom, to)
        transfer(address(this).balance, msg.sender);
        return;
    }

    function withdraw() public {
        // require (owner == msg.sender);
        boolean res = only_Owner();
        if (res) {
            // codigo
        } else {
            // error
        }
        return;
    }

    mapping (address => boolean) isBanned;
    mapping (address => uint256) balances;

    function banAddress(address add) public onlyOwner {
        isBanned[add] = true;
    }

    function deposit() public payable returns(uint256) {
        // REQUIRES
        // address is not banned
        require (!isBanned[msg.sender], "you are banned");

        balances[msg.sender] += msg.value;
        

    }

    mapping (uint256 => uint256) products;
    function buyProduct(uint256 idProd) public returns(boolean){
        require(balances[msg.sender] >= products[idProd]);

        balances[msg.sender] -= products[idProd];
        return true;


    }
}