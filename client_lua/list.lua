function data()
    grouplist = {
        {1, 1, 0, "单机线路"}
    }

    serlist = {
        {"127.0.0.1", "9065", "本地单机"}
    }
end

function main()
    data()
    ServerList.SetGroupMaxNum(table.getn(grouplist))
    for i = 1, table.getn(grouplist) do
        ServerList.SetGroupList(i - 1, grouplist[i][1], grouplist[i][2], grouplist[i][3], grouplist[i][4])
    end
    for i = 1, table.getn(serlist) do
        ServerList.SetServerList(i - 1, serlist[i][1], serlist[i][2], serlist[i][3])
    end
end
