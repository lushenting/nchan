//don't edit this please, it was auto-generated

typedef struct {
  //input: keys: [],  values: [ channel_id ]
  //output: channel_hash {ttl, time_last_seen, subscribers, messages} or nil
  // delete this channel and all its messages
  char *delete;

  //input: keys: [],  values: [ channel_id ]
  //output: channel_hash {ttl, time_last_seen, subscribers, messages} or nil
  // finds and return the info hash of a channel, or nil of channel not found
  char *find_channel;

  //input:  keys: [], values: [channel_id, msg_time, msg_tag, no_msgid_order, create_channel_ttl, subscriber_channel]
  //output: result_code, msg_time, msg_tag, message, content_type,  channel_subscriber_count
  // no_msgid_order: 'FILO' for oldest message, 'FIFO' for most recent
  // create_channel_ttl - make new channel if it's absent, with ttl set to this. 0 to disable.
  // result_code can be: 200 - ok, 404 - not found, 410 - gone, 418 - not yet available
  char *get_message;

  //input:  keys: [], values: [channel_id, time, message, content_type, msg_ttl, max_msg_buf_size subscriber_channel]
  //output: message_tag, channel_hash {ttl, time_last_seen, subscribers, messages}
  char *publish;

  //input:  keys: [], values: [channel_id, status_code]
  //output: current_subscribers
  char *publish_status;

  //input:  keys: [], values: [channel_id, subscriber_delta, channel_empty_ttl, channel_active_ttl]
  //output: current_subscribers
  char *subscriber_count;

} nhpm_redis_lua_scripts_t;

static nhpm_redis_lua_scripts_t nhpm_rds_lua_hashes = {
  "bdcf8ff90e51362024b7cad05711380c9a44e2f3",
  "44b5b03430a7fe8114f74aa247f7ce5cdc572824",
  "80c9b0d78b96f1b96782e4c6d526dc91fd00706a",
  "30a224f8fef5b12b7ef5a09df43766429b381503",
  "12ed3f03a385412690792c4544e4bbb393c2674f",
  "8c7941549bab32b74f42edd169a3119a6da1f752"
};

#define REDIS_LUA_HASH_LENGTH 40

static nhpm_redis_lua_scripts_t nhpm_rds_lua_script_names = {
  "delete",
  "find_channel",
  "get_message",
  "publish",
  "publish_status",
  "subscriber_count",
};

static nhpm_redis_lua_scripts_t nhpm_rds_lua_scripts = {
  //delete
  "--input: keys: [],  values: [ channel_id ]\n"
  "--output: channel_hash {ttl, time_last_seen, subscribers, messages} or nil\n"
  "-- delete this channel and all its messages\n"
  "local id = ARGV[1]\n"
  "local key_msg=    'channel:msg:%s:'..id --not finished yet\n"
  "local key_channel='channel:'..id\n"
  "local messages=   'channel:messages:'..id\n"
  "local subscribers='channel:subscribers:'..id\n"
  "local pubsub=     'channel:pubsub:'..id\n"
  "\n"
  "local enable_debug=true\n"
  "local dbg = (function(on)\n"
  "if on then return function(...) \n"
  "local arg, cur = {...}, nil\n"
  "for i = 1, #arg do\n"
  "  arg[i]=tostring(arg[i])\n"
  "end\n"
  "redis.call('echo', table.concat(arg))\n"
  "  end; else\n"
  "    return function(...) return; end\n"
  "  end\n"
  "end)(enable_debug)\n"
  "\n"
  "dbg(' ####### DELETE #######')\n"
  "local num_messages = 0\n"
  "--delete all the messages right now mister!\n"
  "local msg\n"
  "while true do\n"
  "  msg = redis.call('LPOP', messages)\n"
  "  if msg then\n"
  "    num_messages = num_messages + 1\n"
  "    redis.call('DEL', key_msg:format(msg))\n"
  "  else\n"
  "    break\n"
  "  end\n"
  "end\n"
  "\n"
  "local del_msg=\"delete:\" .. id\n"
  "for k,channel_key in pairs(redis.call('SMEMBERS', subscribers)) do\n"
  "  redis.call('PUBLISH', channel_key, del_msg)\n"
  "end\n"
  "\n"
  "local nearly_departed = nil\n"
  "if redis.call('EXISTS', key_channel) ~= 0 then\n"
  "  nearly_departed = redis.call('hmget', key_channel, 'ttl', 'time_last_seen', 'subscribers')\n"
  "  for i = 1, #nearly_departed do\n"
  "    nearly_departed[i]=tonumber(nearly_departed[i]) or 0\n"
  "  end\n"
  "  \n"
  "  --leave some crumbs behind showing this channel was just deleted\n"
  "  redis.call('setex', \"channel:deleted:\"..id, 5, 1)\n"
  "  \n"
  "  table.insert(nearly_departed, num_messages)\n"
  "end\n"
  "\n"
  "redis.call('DEL', key_channel, messages, subscribers)\n"
  "\n"
  "if redis.call('PUBSUB','NUMSUB', pubsub)[2] > 0 then\n"
  "  redis.call('PUBLISH', pubsub, \"delete\")\n"
  "end\n"
  "\n"
  "return nearly_departed",

  //find_channel
  "--input: keys: [],  values: [ channel_id ]\n"
  "--output: channel_hash {ttl, time_last_seen, subscribers, messages} or nil\n"
  "-- finds and return the info hash of a channel, or nil of channel not found\n"
  "local id = ARGV[1]\n"
  "local key_channel='channel:'..id\n"
  "\n"
  "local enable_debug=true\n"
  "local dbg = (function(on)\n"
  "if on then return function(...) redis.call('echo', table.concat({...})); end\n"
  "  else return function(...) return; end end\n"
  "end)(enable_debug)\n"
  "\n"
  "if redis.call('EXISTS', key_channel) ~= 0 then\n"
  "  local ch = redis.call('hmget', key_channel, 'ttl', 'time_last_seen', 'subscribers')\n"
  "  for i = 1, #ch do\n"
  "    ch[i]=tonumber(ch[i]) or 0\n"
  "  end\n"
  "  table.insert(ch, redis.call('llen', \"channel:messages:\"..id))\n"
  "  return ch\n"
  "else\n"
  "  return nil\n"
  "end",

  //get_message
  "--input:  keys: [], values: [channel_id, msg_time, msg_tag, no_msgid_order, create_channel_ttl, subscriber_channel]\n"
  "--output: result_code, msg_time, msg_tag, message, content_type,  channel_subscriber_count\n"
  "-- no_msgid_order: 'FILO' for oldest message, 'FIFO' for most recent\n"
  "-- create_channel_ttl - make new channel if it's absent, with ttl set to this. 0 to disable.\n"
  "-- result_code can be: 200 - ok, 404 - not found, 410 - gone, 418 - not yet available\n"
  "local id, time, tag, subscribe_if_current = ARGV[1], tonumber(ARGV[2]), tonumber(ARGV[3])\n"
  "local no_msgid_order=ARGV[4]\n"
  "local create_channel_ttl=tonumber(ARGV[5]) or 0\n"
  "local subscriber_channel = ARGV[6]\n"
  "local msg_id\n"
  "if time and time ~= 0 and tag then\n"
  "  msg_id=(\"%s:%s\"):format(time, tag)\n"
  "end\n"
  "\n"
  "local key={\n"
  "  next_message= 'channel:msg:%s:'..id, --not finished yet\n"
  "  message=      'channel:msg:%s:%s', --not done yet\n"
  "  channel=      'channel:'..id,\n"
  "  messages=     'channel:messages:'..id,\n"
  "  pubsub=       'channel:subscribers:'..id\n"
  "}\n"
  "\n"
  "local subscribe = function(unsub)\n"
  "  if subscriber_channel and #subscriber_channel>0 then\n"
  "    --subscribe to this channel.\n"
  "    redis.call(unsub and 'SREM' or 'SADD',  key.pubsub, subscriber_channel)\n"
  "  end\n"
  "end\n"
  "\n"
  "local enable_debug=true\n"
  "local dbg = (function(on)\n"
  "  if on then return function(...) redis.call('echo', table.concat({...})); end\n"
  "  else return function(...) return; end end\n"
  "end)(enable_debug)\n"
  "\n"
  "dbg(' #######  GET_MESSAGE ######## ')\n"
  "\n"
  "local oldestmsg=function(list_key, old_fmt)\n"
  "  local old, oldkey\n"
  "  local n, del=0,0\n"
  "  while true do\n"
  "    n=n+1\n"
  "    old=redis.call('lindex', list_key, -1)\n"
  "    if old then\n"
  "      oldkey=old_fmt:format(old)\n"
  "      local ex=redis.call('exists', oldkey)\n"
  "      if ex==1 then\n"
  "        return oldkey\n"
  "      else\n"
  "        redis.call('rpop', list_key)\n"
  "        del=del+1\n"
  "      end \n"
  "    else\n"
  "      dbg(list_key, \" is empty\")\n"
  "      break\n"
  "    end\n"
  "  end\n"
  "end\n"
  "\n"
  "local tohash=function(arr)\n"
  "  if type(arr)~=\"table\" then\n"
  "    return nil\n"
  "  end\n"
  "  local h = {}\n"
  "  local k=nil\n"
  "  for i, v in ipairs(arr) do\n"
  "    if k == nil then\n"
  "      k=v\n"
  "    else\n"
  "      --dbg(k..\"=\"..v)\n"
  "      h[k]=v; k=nil\n"
  "    end\n"
  "  end\n"
  "  return h\n"
  "end\n"
  "\n"
  "if no_msgid_order ~= 'FIFO' then\n"
  "  no_msgid_order = 'FILO'\n"
  "end\n"
  "\n"
  "local channel = tohash(redis.call('HGETALL', key.channel))\n"
  "local new_channel = false\n"
  "if next(channel) == nil then\n"
  "  if create_channel_ttl==0 then\n"
  "    return {404, nil}\n"
  "  end\n"
  "  redis.call('HSET', key.channel, 'time', time)\n"
  "  redis.call('EXPIRE', key.channel, create_channel_ttl)\n"
  "  channel = {time=time}\n"
  "  new_channel = true\n"
  "end\n"
  "\n"
  "local subs_count = tonumber(channel.subscribers)\n"
  "\n"
  "local found_msg_id\n"
  "if msg_id==nil then\n"
  "  if new_channel then\n"
  "    dbg(\"new channel\")\n"
  "    subscribe()\n"
  "    return {418, nil}\n"
  "  else\n"
  "    dbg(\"no msg id given, ord=\"..no_msgid_order)\n"
  "    \n"
  "    if no_msgid_order == 'FIFO' then --most recent message\n"
  "      dbg(\"get most recent\")\n"
  "      found_msg_id=channel.current_message\n"
  "    elseif no_msgid_order == 'FILO' then --oldest message\n"
  "      dbg(\"get oldest\")\n"
  "      \n"
  "      found_msg_id=oldestmsg(key.messages, ('channel:msg:%s:'..id))\n"
  "    end\n"
  "    if found_msg_id == nil then\n"
  "      --we await a message\n"
  "      subscribe()\n"
  "      return {418, nil}\n"
  "    else\n"
  "      msg_id = found_msg_id\n"
  "      local msg=tohash(redis.call('HGETALL', msg_id))\n"
  "      subscribe('unsub')\n"
  "      if not next(msg) then --empty\n"
  "        return {404, nil}\n"
  "      else\n"
  "        dbg((\"found msg %i:%i  after %i:%i\"):format(msg.time, msg.tag, time, tag))\n"
  "        return {200, tonumber(msg.time) or \"\", tonumber(msg.tag) or \"\", msg.data or \"\", msg.content_type or \"\", subs_count}\n"
  "      end\n"
  "    end\n"
  "  end\n"
  "else\n"
  "\n"
  "  if msg_id and channel.current_message == msg_id\n"
  "   or not channel.current_message then\n"
  "    subscribe()\n"
  "    return {418, nil}\n"
  "  end\n"
  "\n"
  "  key.message=key.message:format(msg_id, id)\n"
  "  local msg=tohash(redis.call('HGETALL', key.message))\n"
  "\n"
  "  if next(msg) == nil then -- no such message. it might've expired, or maybe it was never there\n"
  "    dbg(\"MESSAGE NOT FOUND\")\n"
  "    --subscribe if necessary\n"
  "    if subscriber_channel and #subscriber_channel>0 then\n"
  "      --subscribe to this channel.\n"
  "      redis.call('SADD',  key.pubsub, subscriber_channel)\n"
  "    end\n"
  "    return {404, nil}\n"
  "  end\n"
  "\n"
  "  local next_msg, next_msgtime, next_msgtag\n"
  "  if not msg.next then --this should have been taken care of by the channel.current_message check\n"
  "    dbg(\"NEXT MESSAGE KEY NOT PRESENT. ERROR, ERROR!\")\n"
  "    return {404, nil}\n"
  "  else\n"
  "    dbg(\"NEXT MESSAGE KEY PRESENT: \" .. msg.next)\n"
  "    key.next_message=key.next_message:format(msg.next)\n"
  "    if redis.call('EXISTS', key.next_message)~=0 then\n"
  "      local ntime, ntag, ndata, ncontenttype=unpack(redis.call('HMGET', key.next_message, 'time', 'tag', 'data', 'content_type'))\n"
  "      dbg((\"found msg2 %i:%i  after %i:%i\"):format(ntime, ntag, time, tag))\n"
  "      return {200, tonumber(ntime) or \"\", tonumber(ntag) or \"\", ndata or \"\", ncontenttype or \"\", subs_count}\n"
  "    else\n"
  "      dbg(\"NEXT MESSAGE NOT FOUND\")\n"
  "      return {404, nil}\n"
  "    end\n"
  "  end\n"
  "end",

  //publish
  "--input:  keys: [], values: [channel_id, time, message, content_type, msg_ttl, max_msg_buf_size subscriber_channel]\n"
  "--output: message_tag, channel_hash {ttl, time_last_seen, subscribers, messages}\n"
  "\n"
  "local id=ARGV[1]\n"
  "local time=tonumber(ARGV[2])\n"
  "local msg={\n"
  "  id=nil,\n"
  "  data= ARGV[3],\n"
  "  content_type=ARGV[4],\n"
  "  ttl= tonumber(ARGV[5]),\n"
  "  time= time,\n"
  "  tag= 0\n"
  "}\n"
  "local store_at_most_n_messages = ARGV[6]\n"
  "if store_at_most_n_messages == nil or store_at_most_n_messages == \"\" then\n"
  "  return {err=\"Argument 6, max_msg_buf_size, can't be empty\"}\n"
  "end\n"
  "\n"
  "local enable_debug=true\n"
  "local dbg = (function(on)\n"
  "  if on then return function(...) \n"
  "    local arg, cur = {...}, nil\n"
  "    for i = 1, #arg do\n"
  "      arg[i]=tostring(arg[i])\n"
  "    end\n"
  "    redis.call('echo', table.concat(arg))\n"
  "  end; else\n"
  "    return function(...) return; end\n"
  "  end\n"
  "end)(enable_debug)\n"
  "\n"
  "if type(msg.content_type)=='string' and msg.content_type:find(':') then\n"
  "  return {err='Message content-type cannot contain \":\" character.'}\n"
  "end\n"
  "\n"
  "dbg(' #######  PUBLISH   ######## ')\n"
  "\n"
  "-- sets all fields for a hash from a dictionary\n"
  "local hmset = function (key, dict)\n"
  "  if next(dict) == nil then return nil end\n"
  "  local bulk = {}\n"
  "  for k, v in pairs(dict) do\n"
  "    table.insert(bulk, k)\n"
  "    table.insert(bulk, v)\n"
  "  end\n"
  "  return redis.call('HMSET', key, unpack(bulk))\n"
  "end\n"
  "\n"
  "local tohash=function(arr)\n"
  "  if type(arr)~=\"table\" then\n"
  "    return nil\n"
  "  end\n"
  "  local h = {}\n"
  "  local k=nil\n"
  "  for i, v in ipairs(arr) do\n"
  "    if k == nil then\n"
  "      k=v\n"
  "    else\n"
  "      h[k]=v; k=nil\n"
  "    end\n"
  "  end\n"
  "  return h\n"
  "end\n"
  "\n"
  "local key={\n"
  "  time_offset=  'pushmodule:message_time_offset',\n"
  "  last_message= nil,\n"
  "  message=      'channel:msg:%s:'..id, --not finished yet\n"
  "  channel=      'channel:'..id,\n"
  "  messages=     'channel:messages:'..id,\n"
  "  subscribers=  'channel:subscribers:'..id\n"
  "}\n"
  "local channel_pubsub = 'channel:pubsub:'..id\n"
  "\n"
  "local new_channel\n"
  "local channel\n"
  "if redis.call('EXISTS', key.channel) ~= 0 then\n"
  "  channel=tohash(redis.call('HGETALL', key.channel))\n"
  "end\n"
  "\n"
  "if channel~=nil then\n"
  "  --dbg(\"channel present\")\n"
  "  if channel.current_message ~= nil then\n"
  "    --dbg(\"channel current_message present\")\n"
  "    key.last_message=('channel:msg:%s:%s'):format(channel.current_message, id)\n"
  "  else\n"
  "    --dbg(\"channel current_message absent\")\n"
  "    key.last_message=nil\n"
  "  end\n"
  "  new_channel=false\n"
  "else\n"
  "  --dbg(\"channel missing\")\n"
  "  channel={}\n"
  "  new_channel=true\n"
  "  key.last_message=nil\n"
  "end\n"
  "\n"
  "--set new message id\n"
  "if key.last_message then\n"
  "  local lastmsg = redis.call('HMGET', key.last_message, 'time', 'tag')\n"
  "  local lasttime, lasttag = tonumber(lastmsg[1]), tonumber(lastmsg[2])\n"
  "  --dbg(\"last_time \", lasttime, \" last_tag \", lasttag, \" msg_time \", msg.time)\n"
  "  if lasttime==msg.time then\n"
  "    msg.tag=lasttag+1\n"
  "  end\n"
  "end\n"
  "msg.id=('%i:%i'):format(msg.time, msg.tag)\n"
  "key.message=key.message:format(msg.id)\n"
  "if redis.call('exists', msg.id) ~= 0 then\n"
  "  return {err=\"Message for channel %s id %s already exists\"}\n"
  "end\n"
  "\n"
  "msg.prev=channel.current_message\n"
  "if key.last_message then\n"
  "  redis.call('HSET', key.last_message, 'next', msg.id)\n"
  "end\n"
  "\n"
  "--update channel\n"
  "redis.call('HSET', key.channel, 'current_message', msg.id)\n"
  "if msg.prev then\n"
  "  redis.call('HSET', key.channel, 'prev_message', msg.prev)\n"
  "end\n"
  "if msg.time then\n"
  "  redis.call('HSET', key.channel, 'time', msg.time)\n"
  "end\n"
  "if not channel.ttl then\n"
  "  channel.ttl=msg.ttl\n"
  "  redis.call('HSET', key.channel, 'ttl', channel.ttl)\n"
  "end\n"
  "\n"
  "if not channel.max_stored_messages then\n"
  "  channel.max_stored_messages = store_at_most_n_messages\n"
  "  redis.call('HSET', key.channel, 'max_stored_messages', store_at_most_n_messages)\n"
  "  dbg(\"channel.max_stored_messages was not set, but is now \", store_at_most_n_messages)\n"
  "else\n"
  "  channel.max_stored_messages =tonumber(channel.max_stored_messages)\n"
  "  dbg(\"channel.mas_stored_messages == \" , channel.max_stored_messages)\n"
  "end\n"
  "\n"
  "--write message\n"
  "hmset(key.message, msg)\n"
  "\n"
  "\n"
  "--check old entries\n"
  "local oldestmsg=function(list_key, old_fmt)\n"
  "  local old, oldkey\n"
  "  local n, del=0,0\n"
  "  while true do\n"
  "    n=n+1\n"
  "    old=redis.call('lindex', list_key, -1)\n"
  "    if old then\n"
  "      oldkey=old_fmt:format(old)\n"
  "      local ex=redis.call('exists', oldkey)\n"
  "      if ex==1 then\n"
  "        return oldkey\n"
  "      else\n"
  "        redis.call('rpop', list_key)\n"
  "        del=del+1\n"
  "      end \n"
  "    else\n"
  "      break\n"
  "    end\n"
  "  end\n"
  "end\n"
  "\n"
  "local max_stored_msgs = tonumber(redis.call('HGET', key.channel, 'max_stored_messages')) or -1\n"
  "\n"
  "if max_stored_msgs < 0 then --no limit\n"
  "  oldestmsg(key.messages, 'channel:msg:%s:'..id)\n"
  "  redis.call('LPUSH', key.messages, msg.id)\n"
  "elseif max_stored_msgs > 0 then\n"
  "  local stored_messages = tonumber(redis.call('LLEN', key.messages))\n"
  "  redis.call('LPUSH', key.messages, msg.id)\n"
  "  if stored_messages > max_stored_msgs then\n"
  "    local oldmsgid = redis.call('RPOP', key.messages)\n"
  "    redis.call('DEL', 'channel:msg:'..id..':'..oldmsgid)\n"
  "  end\n"
  "  oldestmsg(key.messages, 'channel:msg:%s:'..id)\n"
  "end\n"
  "\n"
  "\n"
  "\n"
  "--set expiration times for all the things\n"
  "redis.call('EXPIRE', key.message, channel.ttl)\n"
  "redis.call('EXPIRE', key.channel, channel.ttl)\n"
  "redis.call('EXPIRE', key.messages, channel.ttl)\n"
  "--redis.call('EXPIRE', key.subscribers,  channel.ttl)\n"
  "\n"
  "--publish message\n"
  "local unpacked = { time=msg.time, tag=msg.tag, content_type=msg.content_type, channel=id }\n"
  "if #msg.data > 5*1024 then\n"
  "  --we don't want long messages re-printf'd per pubsub channel, just send them the message key, that's nice and short. Also for some reason this stopped working for messages >15Kb, so...\n"
  "  unpacked.key=key.message\n"
  "else\n"
  "  unpacked.data=msg.data\n"
  "end\n"
  "local msgpacked = cmsgpack.pack(unpacked)\n"
  "\n"
  "--dbg((\"Stored message with id %i:%i => %s\"):format(msg.time, msg.tag, msg.data))\n"
  "\n"
  "local subscribers = redis.call('SMEMBERS', key.subscribers)\n"
  "if subscribers and #subscribers > 0 then\n"
  "  for k,channel_key in pairs(subscribers) do\n"
  "    --not efficient, but useful for a few short-term subscriptions\n"
  "    local num=redis.call('PUBLISH', channel_key, msgpacked)\n"
  "    if type(num) == 'table' then\n"
  "      local out={}\n"
  "      for i, v in pairs(table) do\n"
  "        table.insert(out, (\"%s: %s\"):format(tostring(i),tostring(v)))\n"
  "      end\n"
  "      return {err=\"PUBLISHed message \" ..(type(msgpacked)=='string' and \"len:\"..tostring(#msgpacked) or type(msgpacked)) ..  \" to  \"..tostring(channel_key)..\" got reply table {\" .. table.concat(out, \", \") .. \"}\"}\n"
  "    elseif num ~= 1 then\n"
  "      return {err=\"PUBLISHed message \" ..(type(msgpacked)=='string' and \"len:\"..tostring(#msgpacked) or type(msgpacked)) ..  \" to \"..tostring(channel_key)..\" received by \"..tostring(num)..\" clients. Expected just 1.\"}\n"
  "    end\n"
  "  end\n"
  "  --clear short-term subscriber list\n"
  "  redis.call('DEL', key.subscribers)\n"
  "end\n"
  "--now publish to the efficient channel\n"
  "if redis.call('PUBSUB','NUMSUB', channel_pubsub)[2] > 0 then\n"
  "  redis.call('PUBLISH', channel_pubsub, ('%i:%i:%s:%s'):format(msg.time, msg.tag, msg.content_type, msg.data))\n"
  "end\n"
  "\n"
  "local num_messages = redis.call('llen', key.messages);\n"
  "\n"
  "dbg(\"channel \", id, \" ttl: \",channel.ttl, \" subscribers: \", channel.subscribers, \"messages: \", num_messages)\n"
  "return { msg.tag, {tonumber(channel.ttl or msg.ttl), tonumber(channel.time or msg.time), tonumber(channel.subscribers or 0), tonumber(num_messages)}, new_channel}",

  //publish_status
  "--input:  keys: [], values: [channel_id, status_code]\n"
  "--output: current_subscribers\n"
  "local enable_debug=true\n"
  "local dbg = (function(on)\n"
  "if on then\n"
  "  return function(...)\n"
  "  redis.call('echo', table.concat({...}))\n"
  "end\n"
  "  else\n"
  "    return function(...)\n"
  "    return\n"
  "    end\n"
  "  end\n"
  "end)(enable_debug)\n"
  "dbg(' ####### PUBLISH STATUS ####### ')\n"
  "local id=ARGV[1]\n"
  "local code=tonumber(ARGV[2])\n"
  "if code==nil then\n"
  "  return {err=\"non-numeric status code given, bailing!\"}\n"
  "end\n"
  "\n"
  "local pubmsg = \"status:\"..code\n"
  "local subs_key = 'channel:subscribers:'..id\n"
  "local chan_key = 'channel:'..id\n"
  "\n"
  "for k,channel_key in pairs(redis.call('SMEMBERS', subs_key)) do\n"
  "  --not efficient, but useful for a few short-term subscriptions\n"
  "  redis.call('PUBLISH', channel_key, pubmsg)\n"
  "end\n"
  "--clear short-term subscriber list\n"
  "redis.call('DEL', subs_key)\n"
  "--now publish to the efficient channel\n"
  "redis.call('PUBLISH', channel_pubsub, pubmsg)\n"
  "return redis.call('HGET', chan_key, 'subscribers') or 0",

  //subscriber_count
  "--input:  keys: [], values: [channel_id, subscriber_delta, channel_empty_ttl, channel_active_ttl]\n"
  "--output: current_subscribers\n"
  "local id = ARGV[1]\n"
  "local key = 'channel:'..id\n"
  "local subscriber_delta = tonumber(ARGV[2])\n"
  "local channel_empty_ttl = tonumber(ARGV[3]) or 20\n"
  "local channel_active_ttl = tonumber(ARGV[4]) or 0\n"
  "\n"
  "local enable_debug=true\n"
  "local dbg = (function(on)\n"
  "  if on then return function(...) redis.call('echo', table.concat({...})); end\n"
  "  else return function(...) return; end end\n"
  "end)(enable_debug)\n"
  "\n"
  "dbg(' ######## SUBSCRIBER COUNT ####### ')\n"
  "--dbg('active ttl:', type(channel_active_ttl), \" \", tostring(channel_active_ttl), \" empty ttl:\",type(channel_empty_ttl), \" \", tostring(channel_empty_ttl))\n"
  "\n"
  "\n"
  "if not subscriber_delta or subscriber_delta == 0 then\n"
  "  return {err=\"subscriber_delta is not a number or is 0: \" .. type(ARGV[2]) .. \" \" .. tostring(ARGV[2])}\n"
  "end\n"
  "if redis.call('exists', key) == 0 then\n"
  "  if redis.call('exists', \"channel:deleted:\"..id) ~= 0 then\n"
  "    dbg(\"trying to change sub_count for recently deleted channel\")\n"
  "    return 0\n"
  "  else\n"
  "    return {err=(\"%srementing subscriber count for nonexistent channel %s\"):format(subscriber_delta > 0 and \"inc\" or \"dec\", id)}\n"
  "  end\n"
  "end\n"
  "\n"
  "local keys={'channel:'..id, 'channel:messages:'..id, 'channel:subscribers:'..id}\n"
  "local setkeyttl=function(ttl)\n"
  "  for i,v in ipairs(keys) do\n"
  "    if ttl > 0 then\n"
  "      redis.call('expire', v, ttl)\n"
  "    else\n"
  "      redis.call('persist', v)\n"
  "    end\n"
  "  end\n"
  "end\n"
  "\n"
  "local count= redis.call('hincrby', key, 'subscribers', subscriber_delta)\n"
  "if count == 0 and subscriber_delta < 0 then\n"
  "  --dbg(\"this channel now has no subscribers\")\n"
  "  setkeyttl(channel_empty_ttl)\n"
  "elseif count > 0 and count - subscriber_delta == 0 then\n"
  "  --dbg(\"just added subscribers\")\n"
  "  setkeyttl(channel_active_ttl)\n"
  "elseif count < 0 then\n"
  "  return {err=\"Subscriber count for channel \" .. id .. \" less than zero: \" .. count}\n"
  "end\n"
  "\n"
  "return count\n"
};

