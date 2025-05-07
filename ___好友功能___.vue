<template>
  <div>
    <h2>个人中心 - 好友管理</h2>
    <el-tabs v-model="activeTab" type="card">
      <!-- 添加好友：通过搜索用户（用户名/昵称/邮箱） -->
      <el-tab-pane label="添加好友" name="search">
        <div style="margin-bottom: 16px;">
          <el-input placeholder="请输入用户名/昵称/邮箱" v-model="searchQuery" style="width: 300px;"></el-input>
          <el-button type="primary" @click="doSearch" style="margin-left: 8px;">搜索</el-button>
        </div>
        <el-table :data="searchResults" style="width: 100%;" v-if="searchResults.length">
          <el-table-column prop="username" label="用户名"></el-table-column>
          <el-table-column prop="nickname" label="昵称"></el-table-column>
          <el-table-column prop="email" label="邮箱"></el-table-column>
          <el-table-column label="操作">
            <template #default="scope">
              <el-button type="primary" size="small" :disabled="scope.row.requestSent"
                @click="handleAddFriend(scope.row)">
                {{ scope.row.requestSent ? '请求已发送' : '添加好友' }}
              </el-button>
            </template>
          </el-table-column>
        </el-table>
        <div v-else style="text-align: center; margin-top: 20px;">暂无搜索结果</div>
      </el-tab-pane>

      <!-- 待处理请求 -->
      <el-tab-pane label="待处理请求" name="pending">
        <el-button type="primary" @click="loadPendingRequests" style="margin-bottom: 16px;">刷新</el-button>
        <el-table :data="pendingRequests" style="width: 100%;" v-if="pendingRequests.length">
          <el-table-column prop="senderUsername" label="用户名"></el-table-column>
          <el-table-column prop="senderNickname" label="昵称"></el-table-column>
          <el-table-column label="请求时间">
            <template #default="scope">
              <span>{{ formatDate(scope.row.createTime) }}</span>
            </template>
          </el-table-column>
          <el-table-column label="操作">
            <template #default="scope">
              <el-button type="success" size="small" @click="handleAcceptRequest(scope.row)">同意</el-button>
              <el-button type="danger" size="small" @click="handleRejectRequest(scope.row)">拒绝</el-button>
            </template>
          </el-table-column>
        </el-table>
        <div v-else style="text-align: center; margin-top: 20px;">暂无待处理请求</div>
      </el-tab-pane>

      <!-- 好友列表 -->
      <el-tab-pane label="好友列表" name="friends">
        <el-button type="primary" @click="loadFriendList" style="margin-bottom: 16px;">刷新</el-button>
        <el-table :data="friendList" style="width: 100%;" v-if="friendList.length">
          <el-table-column prop="username" label="用户名"></el-table-column>
          <el-table-column prop="nickname" label="昵称"></el-table-column>
          <el-table-column prop="email" label="邮箱"></el-table-column>
          <el-table-column label="操作">
            <template #default="scope">
              <el-button type="danger" size="small" @click="handleRemoveFriend(scope.row)">解除好友</el-button>
            </template>
          </el-table-column>
        </el-table>
        <div v-else style="text-align: center; margin-top: 20px;">暂无好友</div>
      </el-tab-pane>
    </el-tabs>
  </div>
</template>

<script setup lang="ts">
  import { ref, onMounted } from 'vue'
  import { ElMessage } from 'element-plus'
  import { useCounterStore } from '@/stores/counter'
  import { searchUsers } from '@/api/user'
  import { sendFriendRequest, getPendingRequests, acceptFriendRequest, rejectFriendRequest,getFriendList, removeFriend } from '@/api/userfriend'

  const store = useCounterStore()
  const userId = store.userInfo.id

  // Tab 切换状态，默认显示添加好友
  const activeTab = ref('search')
  // 搜索相关
  const searchQuery = ref('')
  const searchResults = ref < any[] > ([])

  // 待处理的好友请求列表（返回 DTO，包含 senderUsername, senderNickname, createTime 等信息）
  const pendingRequests = ref < any[] > ([])

  // 好友列表
  const friendList = ref < any[] > ([])

  /** 格式化日期 */
  const formatDate = (date: any) => {
    return new Date(date).toLocaleString()
  }

  /** 搜索用户 */
  const doSearch = async () => {
    if (!searchQuery.value.trim()) {
      ElMessage.warning('请输入搜索关键词')
      return
    }
    try {
      const res = await searchUsers({ keyword: searchQuery.value })
      searchResults.value = res.data || res
      if (searchResults.value.length === 0) {
        ElMessage.info('未找到相关用户')
      }
      // 初始化每个用户的 requestSent 标识为 false
      searchResults.value.forEach((user: any) => {
        user.requestSent = false
      })
    } catch (error) {
      console.error(error)
      ElMessage.error('搜索失败')
    }
  }

  /** 添加好友 */
  const handleAddFriend = (user: any) => {
    if (user.id === userId) {
      ElMessage.warning('不能添加自己为好友')
      return
    }
    // 判断是否已经标记为已发送
    if (user.requestSent) {
      ElMessage.info('好友请求已发送')
      return
    }
    sendFriendRequest({ senderId: userId, receiverId: user.id })
      .then((res: any) => {
        if (res.code === 500) {
          ElMessage.error(res.message || '发送好友请求失败')
        } else {
          user.requestSent = true
          ElMessage.success('好友请求已发送')
        }
      })
      .catch((err: any) => {
        ElMessage.error(err.response?.data?.message || '发送好友请求失败')
      })
  }

  /** 加载待处理好友请求 */
  const loadPendingRequests = async () => {
    try {
      const res = await getPendingRequests({ userId })
      pendingRequests.value = res.data || res
    } catch (error) {
      console.error(error)
      ElMessage.error('加载待处理请求失败')
    }
  }

  /** 同意好友请求 */
  const handleAcceptRequest = async (request: any) => {
    try {
      await acceptFriendRequest({ requestId: request.requestId, currentUserId: userId })
      ElMessage.success('已同意好友请求')
      loadPendingRequests()
      loadFriendList()
    } catch (error) {
      console.error(error)
      ElMessage.error('同意请求失败')
    }
  }

  /** 拒绝好友请求 */
  const handleRejectRequest = async (request: any) => {
    try {
      await rejectFriendRequest({ requestId: request.requestId, currentUserId: userId })
      ElMessage.success('已拒绝好友请求')
      loadPendingRequests()
    } catch (error) {
      console.error(error)
      ElMessage.error('拒绝请求失败')
    }
  }

  /** 加载好友列表 */
  const loadFriendList = async () => {
    try {
      const res = await getFriendList({ userId })
      friendList.value = res.data || res
    } catch (error) {
      console.error(error)
      ElMessage.error('加载好友列表失败')
    }
  }

  /** 解除好友关系 */
  const handleRemoveFriend = async (friend: any) => {
    try {
      await removeFriend({ userId, friendId: friend.id })
      ElMessage.success('已解除好友关系')
      loadFriendList()
    } catch (error) {
      console.error(error)
      ElMessage.error('解除好友失败')
    }
  }

  // 组件加载后，自动刷新待处理请求和好友列表
  onMounted(() => {
    loadPendingRequests()
    loadFriendList()
  })
</script>